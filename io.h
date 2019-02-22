/*
** Copyright (C) 2001-2013 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**
**	 * Redistributions of source code must retain the above copyright
**	   notice, this list of conditions and the following disclaimer.
**	 * Redistributions in binary form must reproduce the above copyright
**	   notice, this list of conditions and the following disclaimer in
**	   the documentation and/or other materials provided with the
**	   distribution.
**	 * Neither the author nor the names of any contributors may be used
**	   to endorse or promote products derived from this software without
**	   specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
** OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
** OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
** ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include  <stdio.h>
#include  <sndfile.h>

#define    BUFFER_LEN  4096
#define    MAX_CHANNELS  1 /* only mono files are accepted */

int read_complete_file (int sent_L, double * indata, SNDFILE * infile) {

    double databuffer[BUFFER_LEN];
    int Nsamples = 0;
    int i, readcount;

    while ((readcount = sf_read_double (infile, databuffer, BUFFER_LEN)))
    {
        for(i=0; i<readcount; i++)
        {
            indata[i + Nsamples] = databuffer[i];
        }
        Nsamples = Nsamples + readcount;
    }

    return 0;
}


double * get_data(const char * infilename, SF_INFO * p_sfinfo )  {
    SNDFILE *infile;
    int sent_L;
    double * data;

    /* Here's where we open the input file. We pass sf_open the file name and
    ** a pointer to an SF_INFO struct.
    ** On successful open, sf_open returns a SNDFILE* pointer which is used
    ** for all subsequent operations on that file.
    ** If an error occurs during sf_open, the function returns a NULL pointer.
    **
    ** If you are trying to open a raw headerless file you will need to set the
    ** format and channels fields of sfinfo before calling sf_open(). For
    ** instance to open a raw 16 bit stereo PCM file you would need the following
    ** two lines:
    **
    **   sfinfo.format   = SF_FORMAT_RAW | SF_FORMAT_PCM_16 ;
    **   sfinfo.channels = 2 ;
    */
    if (! (infile = sf_open (infilename, SFM_READ, p_sfinfo))) {
      /* Open failed so print an error message. */
      printf ("Not able to open input file %s.\n", infilename) ;
      /* Print the error message from libsndfile. */
      puts (sf_strerror (NULL));
      return  NULL ;
    }

    if (p_sfinfo->channels > MAX_CHANNELS) {
      printf ("Not able to process more than %d channels\n", MAX_CHANNELS) ;
      return  NULL ;
    }

    // the sfinfo.frames field contains the number of samples of the file
    sent_L = p_sfinfo->frames;
    //printf("sent_L: %d\n", sent_L); // **** debug
    data = (double *)malloc(sent_L * sizeof(double));

    if (read_complete_file(sent_L, data, infile))
    {
      /* Error while reading data. */
      printf ("Error while reading input data\n") ;
      /* Print the error message from libsndfile. */
      puts (sf_strerror (NULL)) ;
      return  NULL ;
    }

    /* Close input file */
    sf_close (infile) ;
    return data;
}

int write_data(const char * outfilename, SF_INFO * p_sfinfo, double * data, int length)
{
    SNDFILE  *outfile ;

    /* Open the output file. */
    if (! (outfile = sf_open (outfilename, SFM_WRITE, p_sfinfo)))
    {   printf ("Not able to open output file %s.\n", outfilename) ;
        puts (sf_strerror (NULL)) ;
        return  1 ;
        } ;

    sf_write_double (outfile, data, length);
    sf_close (outfile);
}
