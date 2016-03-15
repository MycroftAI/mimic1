#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
This script is able to:

- Filter a lexicon removing short words not suitable for LTS rules training.
- Test the performance of a LTS rules model, given a lexicon.
- Prune a lexicon with entries that are predicted correctly by an LTS model

Author: Sergio Oller, 2016
"""
from __future__ import unicode_literals
from __future__ import print_function
from codecs import open

import argparse
from collections import defaultdict



##############################################################################
# This interpreter of a subset of the scheme dialect of lisp is based on:
# Lispy: Scheme Interpreter in Python
#
# (c) Peter Norvig, 2010-14; See http://norvig.com/lispy.html
#
# Modifications: (Sergio Oller - github.com/zeehio)
# - The tokenizer uses deque for faster performance
# - atom unquotes string symbols (useful for parsing lexicon in festival
#   format)
##############################################################################

from collections import deque

Symbol = str  # A Scheme Symbol is implemented as a Python str
List = list  # A Scheme List is implemented as a Python list
Number = (int, float)  # A Scheme Number is implemented as a int or float


def tokenize(chars):
    "Convert a string of characters into a list of tokens."
    return deque(chars.replace('(', ' ( ').replace(')', ' ) ').split())


def parse(program):
    "Read a Scheme expression from a string."
    return read_from_tokens(tokenize(program))


def read_from_tokens(tokens):
    "Read an expression from a sequence of tokens."
    if len(tokens) == 0:
        raise SyntaxError('unexpected EOF while reading')
    token = tokens.popleft()
    if token == '(':
        L = []
        while tokens[0] != ')':
            L.append(read_from_tokens(tokens))
        tokens.popleft()  # pop off ')'
        return L
    elif token == ')':
        raise SyntaxError('unexpected )')
    else:
        return atom(token)


def atom(token):
    "Numbers become numbers; every other token is a symbol."
    try:
        return int(token)
    except ValueError:
        try:
            return float(token)
        except ValueError:
            if token[0] == '"' and token[-1] == '"':
                return Symbol(token[1:-1])
            else:
                return Symbol(token)
#############################################################################


def read_raw_lexicon(filename):
    """ Reads a lexicon in Festival format
    The festival format consists of a text file with one entry per line.
    Each entry has three parts:
        - the word, quoted
        - an optional Part Of Speech
        - The syllabification and phonetic transcription
    Example:
      ("hello" nil (((hh ax) 0) ((l ow) 1)))

    The formatting corresponds to a scheme list, so a scheme interpreter is
    required. We use don't use a full scheme interpreter, but it is enough to
    cover our needs.
    """
    with open(filename, "rt") as fd:
        line = fd.readline().rstrip()
        if line != "MNCL":
            print(line)
            yield parse(line)
        for line in fd:
            yield parse(line)


def read_lexicon(filename):
    """ Converts the lexicon, as parsed by read_lexicon into a
    word->[(part of speech1, phones in syllables1, phones1),
           (part of speech2, phones in syllables2, phones2), ...] dictionary.
    """
    output = defaultdict(list)
    for line in read_raw_lexicon(filename):
        word = line[0]
        pos = line[1]
        syls = line[2]
        flattened_syls = flatten_syls(syls)
        output[word].append((pos, syls, flattened_syls))
    return output

def read_raw_align(filename):
    """ Reads a lex.align """
    with open(filename, "rt") as fd:
        line = fd.readline().rstrip()
        if line != "MNCL":
            yield parse(line)
        for line in fd:
            yield parse(line)


def read_align(filename):
    """ Converts the align, as parsed by read_align into a
    word->[(part of speech1, phones in syllables1, phones1),
           (part of speech2, phones in syllables2, phones2), ...] dictionary.
    """
    output = []
    for line in read_raw_align(filename):
        word = line[0]
        pos = line[1]
        phones = line[2:]
        output.append([word, pos, phones])
    return output



def flatten_syls(syls):
    """ This flattens the syllabification, and marks vowels with the
    corresponding stress mark"""
    output = []
    for syl in syls:
        phones = syl[0]
        stressed = syl[1]
        for phone in phones:
            if phone[0] in "aeiouAEIOU@":
                output.append(phone + str(stressed))
            else:
                output.append(phone)
    return output


def read_lts(filename):
    """ This parses a _lts_rules.scm file. It contains the decision tree
    that is used to map words not present in the lexicon to phonemes."""
    with open(filename, "rt") as fd:
        output = []
        for line in fd.readlines():
            # comment (license, author...)
            if line.startswith(";"):
                continue
            # variable declaration, We don't need it:
            if line.startswith("(set!"):
                continue
            output.append(line)
        # A first parenthesis is added because the (set! line has a
        # parenthesis that we want to keep.
        all_rules = "(" + " ".join(output)
        lts = parse(all_rules)
    return lts


def process_lts(lts):
    """
    The letter to sound rules are a set of decision trees trained to predict
    the phoneme that corresponds to a given character, considering the three
    previous characters and the three following characters.

    This function returns a dictionary. The keys of the dictionary are the
    letters. The values are the decision trees for each letter, expressed
    as a lambda function that takes as input a word and an index and returns
    the phoneme associated to that word[index] in its context.
    """
    output = dict()
    for character_tree in lts:
        character = character_tree[0]
        tree = character_tree[1]
        tree_lambda = eval_tree(tree)
        output[character] = tree_lambda
    return output


def eval_tree(tree):
    """ This function takes an LTS tree as parsed by the scheme interpreter
    and returns a lambda function. The lambda function takes a word and an index
    returning the phoneme associated to that word[index].
    """
    # non-final node: Depending on condition we follow one branch or another
    if len(tree) == 3:
        condition = eval_condition(tree[0])
        fun_yes = eval_tree(tree[1])
        fun_no = eval_tree(tree[2])
        return (lambda word, index: fun_yes(word, index) if
                condition(word, index) else fun_no(word, index))
    # final node, we take the most option with highest probability
    elif len(tree) == 1:
        return lambda word, index: tree[0][-1]
    else:
        # should not happen
        raise NotImplementedError("Tree: {}".format(tree))


def eval_condition(condition):
    "Generates a lambda function for a condition in the scheme decision tree"
    # "n.name is 'p'"
    if (len(condition) == 3 and isinstance(condition[0], str) and
            condition[1] == "is"):
        # n.name means we compare the next letter to condition[2],
        # parse_feat computes the offset given the feature
        offset = parse_feat(condition[0])
        return (lambda word, index: word[index+offset] == condition[2])
    else:
        print(condition)
        raise NotImplementedError("I don't understand the condition: {}".
                                  format(condition))


def parse_feat(feat):
    # n. means next, p. means previous
    offset = 0
    if feat == "name":
        offset = 0
    elif feat == "n.name":
        offset = 1
    elif feat == "n.n.name":
        offset = 2
    elif feat == "n.n.n.name":
        offset = 3
    elif feat == "p.name":
        offset = -1
    elif feat == "p.p.name":
        offset = -2
    elif feat == "p.p.p.name":
        offset = -3
    else:
        raise NotImplementedError("Unknown feature: {}".format(feat))
    return offset


def predict_lex(word, pos=None, lexicon=None, flattened=True):
    """ Predicts the phonetic transcription of word with part of speech `pos`
        using lexicon. If word is not in lexicon returns None.
        `flattened` is a boolean that determines if the prediction should
        be the phones with syllabification or the flattened phones
    """
    if lexicon is None:
        return None
    if flattened:
        homograph_trans = 2
    else:
        homograph_trans = 1
    homographs = lexicon.get(word, [])
    if len(homographs) == 0:
        return None
    elif len(homographs) == 1 or pos is None:
        return homographs[0][homograph_trans]
    else:
        # pos->trans
        homograph_dict = dict([(x[0], x[homograph_trans]) for x in homographs])
        if pos in homograph_dict.keys():
            return homograph_dict[pos]
        else:
            return homographs[0][homograph_trans]


def predict_lts(word, lts_lambda, silences=True):
    """
    To predict using Letter To Sound Rules we pad the word with zeros (as done
    in Festival LTS training. Then we choose the right tree for each character
    in our word and predict the phonemes, that are returned in a list.)
    """
    phonemes = []
    word_list = [0, 0, "#"] + list(word) + ["#", 0, 0]
    for word_index in range(3, len(word_list)-3):
        word_char = word_list[word_index]
        tree_lambda = lts_lambda[word_char]
        phone = tree_lambda(word_list, word_index)
        phonemes.append(phone)
    if silences:
        return phonemes
    else:
        return [x for x in phonemes if x != "_epsilon_"]

def predict_letters(letters, lts_lambda, silences=True):
    """
    To predict using Letter To Sound Rules we pad the word with zeros (as done
    in Festival LTS training. Then we choose the right tree for each character
    in our word and predict the phonemes, that are returned in a list.)
    """
    phonemes = []
    word_list = [0, 0, '#'] + letters + ["#", 0, 0]
    for word_index in range(3, len(word_list)-3):
        word_char = word_list[word_index]
        tree_lambda = lts_lambda[word_char]
        phone = tree_lambda(word_list, word_index)
        phonemes.append(phone)
    if silences:
        return phonemes
    else:
        return [x for x in phonemes if x != "_epsilon_"]



def predict(word, pos=None, lexicon=None, lts=None):
    """To predict a word we use the lexicon and, as fallback the letter to
    sound rules.
    """
    wordpred = predict_lex(word, pos, lexicon)
    if wordpred is None:
        return predict_lts(word, lts)
    else:
        return wordpred

def phone_normalize(phones):
    output = []
    for phone in phones:
        if phone == "_epsilon_":
            continue
        output.extend(phone.split("-"))
    return output

def test_lts(align, lts):
    """Accuracy of the lts applied to lexicon"""
    count_word_right = 0
    count_word_wrong = 0
    for letters, pos, phones  in align:
        translts = predict_letters(letters, lts, silences=True)
        norm_phones = phone_normalize(phones)
        norm_translts = phone_normalize(translts)
        if all([ x==y for (x, y) in zip(norm_phones, norm_translts)]):
            count_word_right += 1
        else:
            print("".join(letters), list(zip(norm_phones, norm_translts)))
            count_word_wrong += 1
    accuracy = count_word_right/(count_word_right + count_word_wrong)
    return accuracy

def prune_lexicon(lexicon, lts):
    """Predicts all the lexicon words with lts rules, keeping in a dictionary
    the words that are not predicted correctly"""
    pruned_lex = defaultdict(list)
    for word, homographs in lexicon.items():
        word_lower = word.lower()
        for (pos, syl, trans) in homographs:
            translts = predict_lts(word_lower, lts, silences=False)
            if trans != translts or len(homographs) > 1:
                pruned_lex[word_lower].append((pos, syl, trans))
    return pruned_lex


def write_syls(syls):
    output = []
    output.append("(")
    for syl in syls:
        output.append("(")
        phones = syl[0]
        output.append("(")
        output.append(" ".join(phones))
        output.append(") ")
        stress = syl[1]
        output.append(str(stress))
        output.append(") ")
    output.append(")")
    return "".join(output)


def write_lex(pruned_lex, filename, flattened=True):
    with open(filename, "w") as fd:
        for word in sorted(pruned_lex.keys()):
            for pos_syls_flattened in pruned_lex[word]:
                (pos, syls, flattened_phones) = pos_syls_flattened
                if flattened:
                    if flattened_phones is None:
                        phones_out = write_syls(syls)
                    else:
                        phones_out = "(" + " ".join(flattened_phones) + " )"
                else:
                    # take syls and convert it to a scheme string
                    raise NotImplementedError("Not needed")
                out = '( "' + word + '" ' + pos + " " + phones_out + ")"
                print(out, file=fd)

def load_and_test_lts(align_fn, lts_rules_fn):
    align = read_align(align_fn)
    lts_raw = read_lts(lts_rules_fn)
    lts = process_lts(lts_raw)
    accuracy = test_lts(align, lts)
    print("LTS word accuracy on train set (cmulex expected ~60%): {:.1%}".
          format(accuracy))


def load_and_prune_lex(lexicon_fn, lts_rules_fn, output_pruned_lex_fn):
    lexicon = read_lexicon(lexicon_fn)
    lts_raw = read_lts(lts_rules_fn)
    lts = process_lts(lts_raw)
    pruned_lex = prune_lexicon(lexicon, lts)
    write_lex(pruned_lex, output_pruned_lex_fn, flattened=True)

def parse_args():
    parser = argparse.ArgumentParser(description='Helpers to create lexicon and LTS rules.')
    parser.add_argument('--prune', dest='prune', action='store_const',
                        const=True, default=None,
                        help='Remove all lexicon entries correctly predicted')
    parser.add_argument('--remove-short', dest='remove_short', action='store_const',
                        const=True, default=None,
                        help='Remove all lexicon entries with 3 words or less for LTS training.')
    parser.add_argument('--test-lts', dest='test_lts', action='store_const',
                        const=True, default=None,
                        help='Test LTS rules.')
    args = parser.parse_args()
    return args

def filter_lexicon(lexicon, minlength=4, lower=True):
    filtered_lex = defaultdict(list)
    for word in lexicon.keys():
        word_filtered = word
        if minlength is not None and len(word) < minlength:
            continue
        if lower is True:
            word_filtered = word_filtered.lower()
        filtered_lex[word_filtered] = lexicon[word]
    return filtered_lex

def load_and_filter_lex_for_lts(lexicon_fn, filtered_lex_fn):
    """ We load the raw lexicon and return a lexicon with words with more than 
        three characters. Ideally we could leave nouns, verbs and adjectives as
        the unknown words we will find will probably belong to one of those 
        categories. 
        See: http://www.festvox.org/docs/manual-2.4.0/festival_13.html#Building-letter-to-sound-rules
    """
    lexicon = read_lexicon(lexicon_fn)
    filtered_lex = filter_lexicon(lexicon)
    write_lex(filtered_lex, filtered_lex_fn, flattened=True)


if __name__ == "__main__":
    args = parse_args()
    if args.prune:
        lexicon_fn = "festival/lib/dicts/cmu/cmudict-0.4.out"
        lts_rules_fn = "festival/lib/dicts/cmu/lex_lts_rules.scm"
        output_pruned_lex_fn = "festival/lib/dicts/cmu/pruned_lex.scm"
        load_and_prune_lex(lexicon_fn, lts_rules_fn, output_pruned_lex_fn)
    elif args.remove_short:
        lexicon_fn = "festival/lib/dicts/cmu/cmudict-0.4.out"
        filtered_lex_fn = "festival/lib/dicts/cmu/lts_scratch/lex_entries.out"
        load_and_filter_lex_for_lts(lexicon_fn, filtered_lex_fn)
    elif args.test_lts:
        align_fn = "festival/lib/dicts/cmu/lts_scratch/lex.align"
        lts_rules_fn = "festival/lib/dicts/cmu/lex_lts_rules.scm"
        load_and_test_lts(align_fn, lts_rules_fn)

