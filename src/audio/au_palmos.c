/*************************************************************************/  
/*                                                                       */ 
/*                  Language Technologies Institute                      */ 
/*                     Carnegie Mellon University                        */ 
/*                        Copyright (c) 2004                             */ 
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
/*             Author:  Alan W Black (awb@cs.cmu.edu)                    */ 
/*               Date:  July 2004                                        */ 
/*************************************************************************/ 
/*                                                                       */ 
/*  Audio support for PalmOS                                             */ 
/*                                                                       */ 
/*************************************************************************/ 
    
#include "cst_file.h"
#include <stdlib.h>
#include <sys/types.h>
#include "cst_string.h"
#include "cst_wave.h"
#include "cst_audio.h"
    
#include <System/SoundMgr.h>

{
    



{
    



                                     cst_audiofmt fmt) 
{
    
    
    
    
    
    
    
    
    
    
    
        
    
    
    
#if 0
        if (fmt == CST_AUDIO_LINEAR8 && (sfmts & AFMT_U8))
        
    {
        
        
    
    
    else if (fmt == CST_AUDIO_MULAW && (sfmts & AFMT_MU_LAW))
        
    {
        
        
    
    
    else if (CST_LITTLE_ENDIAN)
        
    {
        
            
        {
            
            
        
        
        else if (sfmts & AFMT_S16_BE)   /* not likely */
            
        {
            
            
            
        
        
        else if (sfmts & AFMT_U8)
            
        {
            
            
        
        
        else
            
        {
            
            
            
        
    
    
    else
        
    {
        
            
        {
            
            
        
        
        else if (sfmts & AFMT_S16_LE)   /* likely */
            
        {
            
            
            
        
        
        else if (sfmts & AFMT_U8)
            
        {
            
            
        
        
        else
            
        {
            
            
            
        
    
    
#endif  /* 
        



{
    
    
        
    
    



{
    
/*    return write((int)ad->platform_data,samples,num_bytes); */ 
        return 0;



{
    
/*    return ioctl((int)ad->platform_data, SNDCTL_DSP_SYNC, NULL); */ 
        return 0;



{
    
/*    return ioctl((int)ad->platform_data, SNDCTL_DSP_RESET, NULL); */ 
        return 0;


