/*
 * file: RTISI.c
 * author: Vicente Almonacid
 *
 * Implements the Real-Time Iterative Spectrum Inversion with Look-Ahead
 * algoithm described in the homonymous paper by Zhu et al.
 * This is a C transcription of a previous Matlab implementation from an unknown
 * author (if you were the author of the original code, contact me so I can
 * give you credit)
*/

#include "RTISI.h"
#include <stdlib.h>
#include <stdio.h>

void process_data (double *data, double *outdata, int sent_L)
{
    int h,i,j, k, kk;   /* iteration vars */
    int prog_percent;
    int L = FRAME_LEN;  /* frame length */
    int S = L/4;        /* hop size */
    int m_S = round(S*SCALE);
    int overlap = L - S;
    int NFrame = floor((sent_L-overlap)/S);
    int L_recon = round(sent_L*SCALE);
    /* Hamming window  */
    float a, b;
    double win[L];
    double U = 0;

    int Nit = 15;
    double xfin[L_recon];
    double frm[L];
    double res[L];
    double x_recon[L];
    double (*x)[2];
    x = malloc(2 * L * sizeof(double));
    double (*xSTFT)[2];
    xSTFT = malloc(2 * L * sizeof(double));
    double xSTFTM[L];

    /* initialization */
    k = kk = 0;
    /* Create Hamming window  */
    a = 0.50;
    b = -0.50;
    for(i=0; i<L; i++){
        win[i] = (sqrt(S)/sqrt((4*a*a+2*b*b)*L))*(a+b*cos(2*PI*(i + 1)/L));
        U = U + win[i];
    }
    U = U / m_S ;

    for(i=0; i<L_recon; i++) xfin[i] = 0;
    for(i=0; i<L; i++){
        frm[i] = res[i] = x_recon[i] = xSTFTM[i] = 0;
        xSTFT[i][0] = xSTFT[i][1] = 0;
    }

    /*
    * Time Stretching Algo
    */
    for(h=0; h<NFrame; h++)
    {
        /* shows progress */
        fflush (stdout) ;
        printf("\r%d %% ", (h+1)*100/NFrame);

        for(i=0; i<L; i++){
                 frm[i] = (win[i]*data[k+i])/U;
                 x[i][0]= frm[i];
                 x[i][1] = 0;
        }

        fft(L,x,xSTFT);

        for(i=0; i<L; i++)
        {
            xSTFTM[i] = sqrt(xSTFT[i][0]*xSTFT[i][0] + xSTFT[i][1]*xSTFT[i][1]);
            xSTFTM[i] = 1.25*xSTFTM[i]; /* there is a 1.25 factor between C and Matlab xSTFTM */
        }

        if (kk+L-1 < L_recon)
        {
            for(i=0; i<L; i++) res[i] = xfin[kk+i];
        }
        else{
            /* out of the expected lenght L_recon, we put zeros... */
            for(i=0; i<L_recon-kk; i++) res[i] = xfin[kk+i];
            for(i=L_recon; i<L+kk; i++) res[i] = 0;
            }

        iterated_recon(x_recon, xSTFTM, res, Nit, win);

        if (kk+L-1 < L_recon){
            for(i=0; i<L; i++) xfin[kk+i]= xfin[kk+i] + x_recon[i];
        }
        else{
            for(i=kk; i<L_recon; i++) xfin[i] = xfin[i] + x_recon[i-kk + 1]; // revisar bounds
        }
        k = k + S;
        kk = kk + m_S;

    }
    for(i=0; i<L_recon; i++) outdata[i] = xfin[i];

    return ;
} /* process_data */


void iterated_recon (double *xrec, double *xSTFTM, double *x_res, int Nit, double *win)
{
    int L = FRAME_LEN ;
    int i,j;
    double randphase;
    double phi[L];
    double frame_est[L];
    /* frame_complex is the same as frame_est,
    *but we need to provide also the imaginary part to the fft
    */
    double (*frame_complex)[2];
    frame_complex = malloc(2 * L * sizeof(double));
    double (*x_rec_complex)[2];
    x_rec_complex = malloc(2 * L * sizeof(double));
    double (*x)[2];
    x = malloc(2 * L * sizeof(double));
    double (*xSTFT)[2];
    xSTFT = malloc(2 * L * sizeof(double));

    for(i=0; i<L;i++) phi[i] = frame_est[i] = 0;

    for(i=0; i<Nit; i++)
    {
        for(j=0; j<L; j++){
            frame_est[j] = win[j]*x_res[j];
            frame_complex[j][0] = frame_est[j];
            frame_complex[j][1] = 0;
        }

        fft(L, frame_complex, xSTFT);

        for(j=0; j<L; j++)
        {
            randphase = (double) rand()*0.01*PI/(double)RAND_MAX;
            phi[j] = atan2(xSTFT[j][1], xSTFT[j][0]) + randphase;
        }

        // adding phase information
        for(j=0; j<L; j++){
            x[j][0]= xSTFTM[j]*cos(phi[j]); // real part
            x[j][1]= xSTFTM[j]*sin(phi[j]); // imaginary part
        }

        for(j=0; j<L; j++) x_rec_complex[j][0] = x_rec_complex[j][1] = 0;
        ifft(L,x_rec_complex,x);
        for(j=0; j<L; j++)x_res[j]= x_rec_complex[j][0]*1.25*1.25; //x_res = real(x_rec)
    }

    for(j=0; j<L; j++) xrec[j] = x_res[j];
}
