/*************************************************************************/
/*                                                                       */
/*                  Language Technologies Institute                      */
/*                     Carnegie Mellon University                        */
/*                      Copyright (c) 2010-2011                          */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*             Author:  Alok Parlikar (aup@cs.cmu.edu)                   */
/*               Date:  April 2010                                       */
/*************************************************************************/
/*                                                                       */
/*  A clustergen generic voice, that can load from a file                */
/*                                                                       */
/*************************************************************************/
#include <stdlib.h>
#include "cst_string.h"
#include "cst_cg_map.h"

#define OPTIMIZE
#ifdef OPTIMIZE
   #define cst_malloc(x, y) malloc(sizeof(x) * y)
   #define OPTIMIZE_DURS
   #define OPTIMIZE_LOAD_DB
   #define OPTIMIZE_DB_READ
   #define OPTIMIZE_READ_NODES
   #define OPTIMIZE_PADDED
#else
   #define cst_malloc(x, y) cst_alloc(x, y)

#endif

const char *const cg_voice_header_string = "CMU_FLITE_CG_VOXDATA-v2.0";

int cst_cg_read_header(cst_file fd)
{
    char header[200];
    size_t n;
    int32_t endianness;

    n = cst_fread(fd, header, sizeof(char),
                  cst_strlen(cg_voice_header_string) + 1);

    if (n < cst_strlen(cg_voice_header_string) + 1)
        return -1;

    if (!cst_streq(header, cg_voice_header_string))
        return -1;

    cst_fread(fd, &endianness, sizeof(int32_t), 1); /* for byte order check */
    if (endianness != cst_endian_loc)
        return -1;              /* dumped with other byte order */

    return 0;
}

char *cst_read_string(cst_file fd)
#ifdef OPTIMIZE_READ_STRING
{
    char *ret;
    int n, numbytes;
    numbytes = cst_read_int32(fd);
    ret = cst_malloc(char, numbytes);
    n = cst_fread(fd, ret, sizeof(char), numbytes);
    if (n != numbytes)
    {
        cst_free(ret);
        return NULL;
    }
 
    return (char *) cst_read_padded(fd, &numbytes);
}
#else
{
    int numbytes;

    return (char *) cst_read_padded(fd, &numbytes);
}
#endif
cst_cg_db *cst_cg_load_db(cst_voice *vox, cst_file fd)
{
    cst_cg_db *db = cst_malloc(cst_cg_db, 1);
    int i;
    uint32_t elements[2];
    uint32_t buff[4];
    db->freeable = 1;           /* somebody can free this if they want */
    db->name = cst_read_string(fd);
    db->types = (const char **) cst_read_db_types(fd);
#ifdef OPTIMIZE_LOAD_DB
    cst_fread(fd, buff, sizeof(uint32_t) * 4, 1);
    db->num_types = buff[0];
    db->sample_rate = buff[1];
    db->f0_mean = ((float *)buff)[2];
    db->f0_stddev = ((float *)buff)[3];
#else
    db->num_types = cst_read_int32(fd);
    db->sample_rate = cst_read_int32(fd);
    db->f0_mean = cst_read_float(fd);
    db->f0_stddev = cst_read_float(fd);
#endif
    db->f0_trees = (const cst_cart **) cst_read_tree_array(fd);

    db->num_param_models =
        get_param_int(vox->features, "num_param_models", 3);
    db->param_trees = cst_malloc(const cst_cart * const *,
                                 db->num_param_models);
    for (i = 0; i < db->num_param_models; i++)
        db->param_trees[i] = (const cst_cart **) cst_read_tree_array(fd);

    db->spamf0 = cst_read_int32(fd);
    if (db->spamf0)
    {
        db->spamf0_accent_tree = cst_read_tree(fd);
        db->spamf0_phrase_tree = cst_read_tree(fd);
    }

    db->num_channels = cst_malloc(int32_t, db->num_param_models);
    db->num_frames = cst_malloc(int32_t, db->num_param_models);
    db->model_vectors =
        cst_malloc(uint16_t **, db->num_param_models);
    for (i = 0; i < db->num_param_models; i++)
    {
#ifdef OPTIMIZE_DB_READ
        cst_fread(fd, elements, sizeof(uint32_t) * 2, 1);
        db->num_channels[i] = elements[0];
        db->num_frames[i] = elements[1];
#else
        db->num_channels[i] = cst_read_int32(fd);
        db->num_frames[i] = cst_read_int32(fd);
#endif
        db->model_vectors[i] =
            (uint16_t **) cst_read_2d_array(fd);
    }
    /* In voices that were built before, they might have NULLs as the */
    /* the vectors ratehr than a real model, so adjust the num_param_models */
    /* accordingly -- this wont cause a leak as there is no alloc'd memory */
    /* in the later unset vectors */
    for (i = 0; i < db->num_param_models; i++)
    {
        if (db->model_vectors[i] == NULL)
            break;
    }
    db->num_param_models = i;

    if (db->spamf0)
    {
        db->num_channels_spamf0_accent = cst_read_int32(fd);
        db->num_frames_spamf0_accent = cst_read_int32(fd);
        db->spamf0_accent_vectors =
            (const float *const *) cst_read_2d_array(fd);
    }

    db->model_min = cst_read_array(fd);
    db->model_range = cst_read_array(fd);

    db->frame_advance = cst_read_float(fd);

    db->num_dur_models = get_param_int(vox->features, "num_dur_models", 1);
    db->dur_stats = cst_malloc(const dur_stat **, db->num_dur_models);
    db->dur_cart = cst_malloc(const cst_cart *, db->num_dur_models);

    for (i = 0; i < db->num_dur_models; i++)
    {
        db->dur_stats[i] = (const dur_stat **) cst_read_dur_stats(fd);
        db->dur_cart[i] = (const cst_cart *) cst_read_tree(fd);
    }

    db->phone_states = (const char *const *const *) cst_read_phone_states(fd);

    db->do_mlpg = cst_read_int32(fd);
    db->dynwin = cst_read_array(fd);
    db->dynwinsize = cst_read_int32(fd);

    db->mlsa_alpha = cst_read_float(fd);
    db->mlsa_beta = cst_read_float(fd);

    db->multimodel = cst_read_int32(fd);
    db->mixed_excitation = cst_read_int32(fd);

    db->ME_num = cst_read_int32(fd);
    db->ME_order = cst_read_int32(fd);
    db->me_h = (const double *const *) cst_read_2d_array(fd);

    db->spamf0 = cst_read_int32(fd);      /* yes, twice, its above too */
    db->gain = cst_read_float(fd);

    return db;

}

void cst_cg_free_db(cst_file fd, cst_cg_db *db)
{
    (void) fd;
    /* Only gets called when this isn't populated : I think */
    cst_free(db);
}

void *cst_read_padded(cst_file fd, int *numbytes)
#ifdef OPTIMIZE_PADDED
{
    void *ret;
    *numbytes = cst_read_int32(fd);
    ret = (void *) cst_malloc(char, *numbytes);
    cst_fread(fd, ret, sizeof(char), *numbytes);
    return ret;
}
#else
{
    void *ret;
    int n;
    *numbytes = cst_read_int32(fd);
    ret = (void *) cst_malloc(char, *numbytes);
    n = cst_fread(fd, ret, sizeof(char), *numbytes);
    if (n != (*numbytes))
    {
        cst_free(ret);
        return NULL;
    }
    return ret;
}
#endif
char **cst_read_db_types(cst_file fd)
{
    char **types;
    int numtypes;
    int i;

    numtypes = cst_read_int32(fd);
    types = cst_malloc(char *, numtypes + 1);

    for (i = 0; i < numtypes; i++)
    {
        types[i] = cst_read_string(fd);
    }
    types[i] = 0;

    return types;
}

cst_cart_node *cst_read_tree_nodes(cst_file fd)
{
    cst_cart_node *nodes;
    int temp;
    int i, num_nodes;
    short vtype;
    char *str;
    uint16_t buff[3];
    num_nodes = cst_read_int32(fd);
    nodes = cst_malloc(cst_cart_node, num_nodes + 1);

    for (i = 0; i < num_nodes; i++)
    {
#ifdef OPTIMIZE_READ_NODES
        cst_fread(fd, buff, sizeof(uint16_t) * 3, 1);
        nodes[i].feat = buff[0] & 0xff;
        nodes[i].op = buff[0] >> 8;
        nodes[i].no_node = buff[1];
        vtype = buff[2];
#else
        cst_fread(fd, &nodes[i].feat, sizeof(char), 1);
        cst_fread(fd, &nodes[i].op, sizeof(char), 1);
        cst_fread(fd, &nodes[i].no_node, sizeof(int16_t), 1);
        cst_fread(fd, &vtype, sizeof(int16_t), 1);
#endif
        if (vtype == CST_VAL_TYPE_STRING)
        {
            str = cst_read_padded(fd, &temp);
            nodes[i].val = string_val(str);
            cst_free(str);
        }
        else if (vtype == CST_VAL_TYPE_INT)
            nodes[i].val = int_val(cst_read_int32(fd));
        else if (vtype == CST_VAL_TYPE_FLOAT)
            nodes[i].val = float_val(cst_read_float(fd));
        else
            nodes[i].val = int_val(cst_read_int32(fd));
    }
    nodes[i].val = NULL;

    return nodes;
}

char **cst_read_tree_feats(cst_file fd)
{
    char **feats;
    int numfeats;
    int i;
    numfeats = cst_read_int32(fd);
    feats = cst_malloc(char *, numfeats + 1);
    
    for (i = 0; i < numfeats; i++)
    {
        feats[i] = (char *)cst_read_string(fd);
    }
    feats[i] = 0;
    return feats;
}

cst_cart *cst_read_tree(cst_file fd)
{
    cst_cart *tree;

    tree = cst_malloc(cst_cart, 1);
    tree->rule_table = cst_read_tree_nodes(fd);
    tree->feat_table = (const char *const *) cst_read_tree_feats(fd);

    return tree;
}

cst_cart **cst_read_tree_array(cst_file fd)
{
    cst_cart **trees = NULL;
    int numtrees;
    int i;

    numtrees = cst_read_int32(fd);

    if (numtrees > 0)
    {
        trees = cst_malloc(cst_cart *, numtrees + 1);

        for (i = 0; i < numtrees; i++)
            trees[i] = cst_read_tree(fd);
        trees[i] = 0;
    }

    return trees;
}

void *cst_read_array(cst_file fd)
{
    int temp;
    void *ret;
    ret = cst_read_padded(fd, &temp);
    return ret;
}

void **cst_read_2d_array(cst_file fd)
{
    int numrows;
    int i;
    void **arrayrows = NULL;
    numrows = cst_read_int32(fd);
    if (numrows > 0)
    {
        arrayrows = cst_malloc(void *, numrows);

        for (i = 0; i < numrows; i++)
        {
            arrayrows[i] = cst_read_array(fd);
        }
    }
    return arrayrows;
}

dur_stat **cst_read_dur_stats(cst_file fd)
{
    int numstats;
    int i, temp;
    dur_stat **ds;
    float elements[2];
    numstats = cst_read_int32(fd);
    ds = cst_malloc(dur_stat *, (1 + numstats));

    /* load structuer values */
    for (i = 0; i < numstats; i++)
    {
        ds[i] = cst_malloc(dur_stat, 1);
#ifdef OPTIMIZE_DURS
        cst_fread(fd, elements, sizeof(uint32_t), 2);
        ds[i]->mean = elements[0];
        ds[i]->stddev = elements[1];
#else
        ds[i]->mean = cst_read_float(fd);
        ds[i]->stddev = cst_read_float(fd);
#endif
        ds[i]->phone = cst_read_padded(fd, &temp);
    }
    ds[i] = NULL;
    return ds;
}

char ***cst_read_phone_states(cst_file fd)
{
    int i, j, count1, count2, temp;
    char ***ps;
    count1 = cst_read_int32(fd);
    ps = cst_malloc(char **, count1 + 1);
    for (i = 0; i < count1; i++)
    {
        count2 = cst_read_int32(fd);
        ps[i] = cst_malloc(char *, count2 + 1);
        for (j = 0; j < count2; j++)
        {
            ps[i][j] = cst_read_padded(fd, &temp);
        }
        ps[i][j] = 0;
    }
    ps[i] = 0;

    return ps;
}

void cst_read_voice_feature(cst_file fd, char **fname, char **fval)
{
    int temp;
    *fname = cst_read_padded(fd, &temp);
    *fval = cst_read_padded(fd, &temp);
}

int32_t cst_read_int32(cst_file fd)
{
    int32_t val = 0;
    cst_fread(fd, &val, sizeof(int32_t), 1);
    return val;
}

float cst_read_float(cst_file fd)
{
    float val = 0;
    cst_fread(fd, &val, sizeof(float), 1);
    return val;
}
