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
#include "cst_string.h"
#include "cst_cg_map.h"

const char * const cg_voice_header_string = "CMU_FLITE_CG_VOXDATA-v2.0";

int cst_cg_read_header(cst_file fd)
{
    char header[200];
    int n, endianness;

    n = cst_fread(fd,header,sizeof(char),cst_strlen(cg_voice_header_string)+1);

    if (n < cst_strlen(cg_voice_header_string)+1)
        return -1;

    if (!cst_streq(header,cg_voice_header_string))
        return -1;

    cst_fread(fd,&endianness,sizeof(int),1); /* for byte order check */
    if (endianness != cst_endian_loc)
        return -1;                           /* dumped with other byte order */
  
    return 0;
}

char *cst_read_string(cst_file fd)
{
    int numbytes;

    return (char *)cst_read_padded(fd,&numbytes);
}

cst_cg_db *cst_cg_load_db(cst_voice *vox,cst_file fd)
{
    cst_cg_db* db = cst_alloc(cst_cg_db,1);
    int i;

    db->freeable = 1;  /* somebody can free this if they want */

    db->name = cst_read_string(fd);
    db->types = (const char**)cst_read_db_types(fd);

    db->num_types = cst_read_int(fd);
    db->sample_rate = cst_read_int(fd);
    db->f0_mean = cst_read_float(fd);
    db->f0_stddev = cst_read_float(fd);

    db->f0_trees = (const cst_cart**) cst_read_tree_array(fd);

    db->num_param_models = get_param_int(vox->features,"num_param_models",3);
    db->param_trees = cst_alloc(const cst_cart **,db->num_param_models);
    for (i=0; i<db->num_param_models; i++)
        db->param_trees[i] = (const cst_cart **) cst_read_tree_array(fd);

    db->spamf0 = cst_read_int(fd);
    if (db->spamf0)
    {
        db->spamf0_accent_tree = cst_read_tree(fd);
        db->spamf0_phrase_tree = cst_read_tree(fd);
    }

    db->num_channels = cst_alloc(int,db->num_param_models);
    db->num_frames = cst_alloc(int,db->num_param_models);
    db->model_vectors = cst_alloc(const unsigned short **,db->num_param_models);
    for (i=0; i<db->num_param_models; i++)
    {
        db->num_channels[i] = cst_read_int(fd);
        db->num_frames[i] = cst_read_int(fd);
        db->model_vectors[i] =
            (const unsigned short **)cst_read_2d_array(fd);
    }
    /* In voices that were built before, they might have NULLs as the */
    /* the vectors ratehr than a real model, so adjust the num_param_models */
    /* accordingly -- this wont cause a leak as there is no alloc'd memory */
    /* in the later unset vectors */
    for (i=0; i<db->num_param_models; i++)
    {
        if (db->model_vectors[i] == NULL)
            break;
    }
    db->num_param_models = i;

    if (db->spamf0)
    {
        db->num_channels_spamf0_accent = cst_read_int(fd);
        db->num_frames_spamf0_accent = cst_read_int(fd);
        db->spamf0_accent_vectors = 
            (const float * const *)cst_read_2d_array(fd);
    }

    db->model_min = cst_read_array(fd);
    db->model_range = cst_read_array(fd);

    db->frame_advance = cst_read_float(fd);

    db->num_dur_models = get_param_int(vox->features,"num_dur_models",1);
    db->dur_stats = cst_alloc(const dur_stat **,db->num_dur_models);
    db->dur_cart = cst_alloc(const cst_cart *,db->num_dur_models);

    for (i=0; i<db->num_dur_models; i++)
    {
        db->dur_stats[i] = (const dur_stat **)cst_read_dur_stats(fd);
        db->dur_cart[i] = (const cst_cart *)cst_read_tree(fd);
    }

    db->phone_states = 
        (const char * const * const *)cst_read_phone_states(fd);

    db->do_mlpg = cst_read_int(fd);
    db->dynwin = cst_read_array(fd);
    db->dynwinsize = cst_read_int(fd);

    db->mlsa_alpha = cst_read_float(fd);
    db->mlsa_beta = cst_read_float(fd);

    db->multimodel = cst_read_int(fd);
    db->mixed_excitation = cst_read_int(fd);

    db->ME_num = cst_read_int(fd);
    db->ME_order = cst_read_int(fd);
    db->me_h = (const double * const *)cst_read_2d_array(fd);
    
    db->spamf0 = cst_read_int(fd); /* yes, twice, its above too */
    db->gain = cst_read_float(fd);

    return db;
  
}

void cst_cg_free_db(cst_file fd, cst_cg_db *db)
{
    /* Only gets called when this isn't populated : I think */ 
    cst_free(db);
}

void *cst_read_padded(cst_file fd, int *numbytes)
{
    void* ret;
    int n; 

    *numbytes = cst_read_int(fd);
    ret = (void *)cst_alloc(char,*numbytes);
    n = cst_fread(fd,ret,sizeof(char),*numbytes);
    if (n != (*numbytes))
    {
        cst_free(ret);
        return NULL;
    }
    return ret;
}

char **cst_read_db_types(cst_file fd)
{
    char** types;
    int numtypes;
    int i;

    numtypes = cst_read_int(fd);
    types = cst_alloc(char*,numtypes+1);
  
    for(i=0;i<numtypes;i++)
    {
        types[i] = cst_read_string(fd);
    }
    types[i] = 0;
  
    return types;
}

cst_cart_node* cst_read_tree_nodes(cst_file fd)
{   
    cst_cart_node* nodes;
    int temp;
    int i, num_nodes;
    short vtype;
    char *str;

    num_nodes = cst_read_int(fd);
    nodes = cst_alloc(cst_cart_node,num_nodes+1);

    for (i=0; i<num_nodes; i++)
    {
        cst_fread(fd,&nodes[i].feat,sizeof(char),1);
        cst_fread(fd,&nodes[i].op,sizeof(char),1);
        cst_fread(fd,&nodes[i].no_node,sizeof(short),1);
        cst_fread(fd,&vtype,sizeof(short),1);
        if (vtype == CST_VAL_TYPE_STRING)
        {
            str = cst_read_padded(fd,&temp);
            nodes[i].val = string_val(str);
            cst_free(str);
        }
        else if (vtype == CST_VAL_TYPE_INT)
            nodes[i].val = int_val(cst_read_int(fd));
        else if (vtype == CST_VAL_TYPE_FLOAT)
            nodes[i].val = float_val(cst_read_float(fd));
        else
            nodes[i].val = int_val(cst_read_int(fd));
    }
    nodes[i].val = NULL;

    return nodes;
}

char** cst_read_tree_feats(cst_file fd)
{
    char** feats;
    int numfeats;
    int i;

    numfeats = cst_read_int(fd);
    feats = cst_alloc(char *,numfeats+1);

    for(i=0;i<numfeats;i++)
        feats[i] = cst_read_string(fd);
    feats[i] = 0;
  
    return feats;
}

cst_cart* cst_read_tree(cst_file fd)
{
    cst_cart* tree;

    tree = cst_alloc(cst_cart,1);
    tree->rule_table = cst_read_tree_nodes(fd);  
    tree->feat_table = (const char * const *)cst_read_tree_feats(fd);

    return tree;
}

cst_cart** cst_read_tree_array(cst_file fd)
{
    cst_cart** trees = NULL;
    int numtrees;
    int i;

    numtrees = cst_read_int(fd);
    
    if (numtrees > 0)
    {
        trees = cst_alloc(cst_cart *,numtrees+1);

        for(i=0;i<numtrees;i++)
            trees[i] = cst_read_tree(fd);
        trees[i] = 0;
    }

    return trees; 
}

void* cst_read_array(cst_file fd)
{
    int temp;
    void* ret;
    ret = cst_read_padded(fd,&temp);
    return ret;
}

void** cst_read_2d_array(cst_file fd)
{
    int numrows;
    int i;
    void** arrayrows = NULL;

    numrows = cst_read_int(fd);

    if (numrows > 0)
    {
        arrayrows = cst_alloc(void *,numrows);

        for(i=0;i<numrows;i++)
            arrayrows[i] = cst_read_array(fd);
    }

    return arrayrows; 
}

dur_stat** cst_read_dur_stats(cst_file fd)
{
    int numstats;
    int i,temp;
    dur_stat** ds;

    numstats = cst_read_int(fd);
    ds = cst_alloc(dur_stat *,(1+numstats));

    /* load structuer values */
    for(i=0;i<numstats;i++)
    {
        ds[i] = cst_alloc(dur_stat,1);
        ds[i]->mean = cst_read_float(fd);
        ds[i]->stddev = cst_read_float(fd);
        ds[i]->phone = cst_read_padded(fd,&temp);
    }
    ds[i] = NULL;

    return ds;
}

char*** cst_read_phone_states(cst_file fd)
{
    int i,j,count1,count2,temp;
    char*** ps;

    count1 = cst_read_int(fd);
    ps = cst_alloc(char **,count1+1);
    for(i=0;i<count1;i++)
    {
        count2 = cst_read_int(fd);
        ps[i] = cst_alloc(char *,count2+1);
        for(j=0;j<count2;j++)
	{
            ps[i][j]=cst_read_padded(fd,&temp);
	}
        ps[i][j] = 0;
    }
    ps[i] = 0;

    return ps;
}

void cst_read_voice_feature(cst_file fd,char** fname, char** fval)
{
    int temp;
    *fname = cst_read_padded(fd,&temp);
    *fval = cst_read_padded(fd,&temp);
}

int cst_read_int(cst_file fd)
{
    int val;
    int n;

    n = cst_fread(fd,&val,sizeof(int),1);
    if (n != 1)
        return 0;
    return val;
}

float cst_read_float(cst_file fd)
{
    float val;
    int n;

    n = cst_fread(fd,&val,sizeof(float),1);
    if (n != 1)
        return 0;
    return val;
}
