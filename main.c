/*
 * file: main.c
 * author: Vicente Almonacid
 *
 * This program uses the Real-Time Iterative Spectrum Inversion with Look-Ahead
 * (RTISI-LA) algorithm to stretch an audio file.
 * It is was developed for experimentation purposes within the context of a
 * R&D collaboration project between engineering students of TELECOM Bretagne
 * and Renesas Design France.
 *
 * Input/output functions use code by Erik de Castro Lopo.
 * FFT routine by Jerome R. Breitenbach.
 * The original matlab implementation of RTISI-LA is from an unknown author.
 *
*/


/* functions to manage input and output operations with sndfile */
#include    "io.h"
/* functions that concern only the algorithm */
#include    "RTISI.h"

int main (void)
{
    double * indata;  /* input data to be proccesed  */
    double * outdata; /* Out data buffer */
    int sent_L;       /* input data length */
    int L_recon;      /* output data length */
    int i;

    SF_INFO sfinfo ;
    int readcount ;
    const char *infilename = "input.wav" ;
    const char *outfilename = "output.wav" ;

    indata = get_data(infilename, &sfinfo); /* read the file and puts the data in the array */
    if(indata == NULL) return 1;
    sent_L = sfinfo.frames;  /* length is given by the field frames of the sfinfo structure */
    L_recon = round(sent_L*SCALE);
    outdata = (double *)malloc(L_recon * sizeof(double));

    //print some file information
    printf("File properties:\n\n");
    printf("\tframes: %d\n", sfinfo.frames);
    printf("\tsamplerate: %d\n", sfinfo.samplerate);
    printf("\tchannels: %d\n", sfinfo.channels);
    printf("\tformat: %d\n", sfinfo.format);
    printf("\tsections: %d\n", sfinfo.sections);
    printf("\tseekable: %d\n", sfinfo.seekable);
    printf("\n");


    /* here we call the time stretching algo */
    printf("\nStarting processing...\n");
    process_data (indata, outdata, sent_L) ;
    printf("\nWriting output...\n");
    write_data(outfilename, &sfinfo, outdata, L_recon);
    printf("\nComplete.\n");

    /* write output samples for debugging */
    FILE * fp;
    fp = fopen ("output.txt", "w");
    for (i=0; i<L_recon; i++) fprintf(fp, "%lf\n",i, outdata[i]);
    fclose( fp );

    system("pause");

    return 0 ;
}
