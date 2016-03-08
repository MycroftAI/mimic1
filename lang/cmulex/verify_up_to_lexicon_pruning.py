#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
View the pruned lexicon and the letter to sound rules to assess
that they work properly.

Author: Sergio Oller, 2016
"""


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
    if '(' == token:
        L = []
        while tokens[0] != ')':
            L.append(read_from_tokens(tokens))
        tokens.popleft()  # pop off ')'
        return L
    elif ')' == token:
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


def read_lexicon(filename):
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


def flatten_lexicon(lexicon):
    """ Converts the lexicon, as parsed by read_lexicon into a
    word->transcription dict.
    In this conversion process the syllabification is flattened, as we don't
    want it.
    """
    output = dict()
    for line in lexicon:
        word = line[0]
        # pos = line[1]
        syls = line[2]
        transcr = flatten_syls(syls)
        output[word] = transcr
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
    """ This parses an _lts_rules.scm file. It contains the decision tree
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
    the phoneme associated to that word.
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
    and returns a lambda function.
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
        options = dict(((y, x) for (x, y) in tree[0][:-1]))
        maxkey = max(options.keys())
        return lambda word, index: options[maxkey]
    else:
        # should not happen
        raise NotImplementedError("Tree: {}".format(tree))


def eval_condition(condition):
    "Evaluates a tree condition"
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


def predict(word, lexicon, lts):
    """To predict a word we use the lexicon and, as fallback the letter to
    sound rules.
    """
    return lexicon.get(word, predict_lts(word, lts))


def predict_lts(word, lts_lambda):
    """
    To predict using Letter To Sound Rules we pad the word with zeros (as done
    in Festival LTS training. Then we choose the right tree for each character
    in our word and predict the phonemes, that are returned in a list.)
    """
    phonemes = []
    word_list = [0, 0, 0] + list(word) + [0, 0, 0]
    for word_index in range(3, len(word_list)-3):
        word_char = word_list[word_index]
        tree_lambda = lts_lambda[word_char]
        phonemes.append(tree_lambda(word_list, word_index))
    return phonemes


# ./make_cmulex setup
# ./make_cmulex lts
# ./make_cmulex lex
lexicon_raw = read_lexicon("festival/lib/dicts/cmu/pruned_lex.out")
lexicon = flatten_lexicon(lexicon_raw)
lts_raw = read_lts("festival/lib/dicts/cmu/cmu_lts_rules.scm")
lts = process_lts(lts_raw)

predict("hello", lexicon, lts)

for x in lexicon.keys():
    try:
        lowerca = x.lower()  # predict_lts works with letters a-z only
        predict_lts(lowerca, lts)
    except:
        print(x)
        raise
