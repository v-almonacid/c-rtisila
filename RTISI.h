/*
 *  file: RTISI.h
 *  author: Vicente Almonacid
*/

#ifndef __RTISI_H__
#define __RTISI_H__

#define  PI 3.1416
#define  SCALE 1.3
#define  FRAME_LEN 256

void process_data (double *data, double *outdata, int sent_L) ;
void iterated_recon(double *xrec, double *xSTFTM, double *x_res, int Nit, double *win) ;

#endif
