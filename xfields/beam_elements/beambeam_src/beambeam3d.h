#ifndef XFIELDS_BEAMBEAM3D_H
#define XFIELDS_BEAMBEAM3D_H

#if !defined(mysign)
    #define mysign(a) (((a) >= 0) - ((a) < 0))
#endif

/*gpufun*/
void Sigmas_propagate(
	Sigmas sigmas_0,
        double const S,
	double const threshold_singular,
        int64_t const handle_singularities,
        double* Sig_11_hat_ptr,
        double* Sig_33_hat_ptr,
        double* costheta_ptr,
        double* sintheta_ptr,
        double* dS_Sig_11_hat_ptr,
        double* dS_Sig_33_hat_ptr,
        double* dS_costheta_ptr,
        double* dS_sintheta_ptr)
{
    double const Sig_11_0 = Sigmas_get_Sig_11(sigmas);
    double const Sig_12_0 = Sigmas_get_Sig_12(sigmas);
    double const Sig_13_0 = Sigmas_get_Sig_13(sigmas);
    double const Sig_14_0 = Sigmas_get_Sig_14(sigmas);
    double const Sig_22_0 = Sigmas_get_Sig_22(sigmas);
    double const Sig_23_0 = Sigmas_get_Sig_23(sigmas);
    double const Sig_24_0 = Sigmas_get_Sig_24(sigmas);
    double const Sig_33_0 = Sigmas_get_Sig_33(sigmas);
    double const Sig_34_0 = Sigmas_get_Sig_34(sigmas);
    double const Sig_44_0 = Sigmas_get_Sig_44(sigmas);

    // Propagate sigma matrix
    double const Sig_11 = Sig_11_0 + 2.*Sig_12_0*S+Sig_22_0*S*S;
    double const Sig_33 = Sig_33_0 + 2.*Sig_34_0*S+Sig_44_0*S*S;
    double const Sig_13 = Sig_13_0 + (Sig_14_0+Sig_23_0)*S+Sig_24_0*S*S;

    double const Sig_12 = Sig_12_0 + Sig_22_0*S;
    double const Sig_14 = Sig_14_0 + Sig_24_0*S;
    double const Sig_22 = Sig_22_0 + 0.*S;
    double const Sig_23 = Sig_23_0 + Sig_24_0*S;
    double const Sig_24 = Sig_24_0 + 0.*S;
    double const Sig_34 = Sig_34_0 + Sig_44_0*S;
    double const Sig_44 = Sig_44_0 + 0.*S;

    double const R = Sig_11-Sig_33;
    double const W = Sig_11+Sig_33;
    double const T = R*R+4*Sig_13*Sig_13;

    //evaluate derivatives
    double const dS_R = 2.*(Sig_12_0-Sig_34_0)+2*S*(Sig_22_0-Sig_44_0);
    double const dS_W = 2.*(Sig_12_0+Sig_34_0)+2*S*(Sig_22_0+Sig_44_0);
    double const dS_Sig_13 = Sig_14_0 + Sig_23_0 + 2*Sig_24_0*S;
    double const dS_T = 2*R*dS_R+8.*Sig_13*dS_Sig_13;

    double Sig_11_hat, Sig_33_hat, costheta, sintheta, dS_Sig_11_hat,
           dS_Sig_33_hat, dS_costheta, dS_sintheta, cos2theta, dS_cos2theta;

    double const signR = mysign(R);


    if (T<threshold_singular && handle_singularities){

        double const a = Sig_12-Sig_34;
        double const b = Sig_22-Sig_44;
        double const c = Sig_14+Sig_23;
        double const d = Sig_24;

        double sqrt_a2_c2 = sqrt(a*a+c*c);

        if (sqrt_a2_c2*sqrt_a2_c2*sqrt_a2_c2 < threshold_singular){
        //equivalent to: if np.abs(c)<threshold_singular and np.abs(a)<threshold_singular:

            if (fabs(d)> threshold_singular){
                cos2theta = fabs(b)/sqrt(b*b+4*d*d);
                }
            else{
                cos2theta = 1.;
                } // Decoupled beam

            costheta = sqrt(0.5*(1.+cos2theta));
            sintheta = mysign(b)*mysign(d)*sqrt(0.5*(1.-cos2theta));

            dS_costheta = 0.;
            dS_sintheta = 0.;

            Sig_11_hat = 0.5*W;
            Sig_33_hat = 0.5*W;

            dS_Sig_11_hat = 0.5*dS_W;
            dS_Sig_33_hat = 0.5*dS_W;
        }
        else{
            //~ printf("I am here\n");
            //~ printf("a=%.2e c=%.2e\n", a, c);
            sqrt_a2_c2 = sqrt(a*a+c*c); //repeated?
            cos2theta = fabs(2.*a)/(2*sqrt_a2_c2);
            costheta = sqrt(0.5*(1.+cos2theta));
            sintheta = mysign(a)*mysign(c)*sqrt(0.5*(1.-cos2theta));

            dS_cos2theta = mysign(a)*(0.5*b/sqrt_a2_c2-a*(a*b+2.*c*d)/(2.*sqrt_a2_c2*sqrt_a2_c2*sqrt_a2_c2));

            dS_costheta = 1./(4.*costheta)*dS_cos2theta;
            if (fabs(sintheta)>threshold_singular){
            //equivalent to: if np.abs(c)>threshold_singular:
                dS_sintheta = -1./(4.*sintheta)*dS_cos2theta;
            }
            else{
                dS_sintheta = d/(2.*a);
            }

            Sig_11_hat = 0.5*W;
            Sig_33_hat = 0.5*W;

            dS_Sig_11_hat = 0.5*dS_W + mysign(a)*sqrt_a2_c2;
            dS_Sig_33_hat = 0.5*dS_W - mysign(a)*sqrt_a2_c2;
        }
    }
    else{

        double const sqrtT = sqrt(T);
        cos2theta = signR*R/sqrtT;
        costheta = sqrt(0.5*(1.+cos2theta));
        sintheta = signR*mysign(Sig_13)*sqrt(0.5*(1.-cos2theta));

        //in sixtrack this line seems to be different different
        // sintheta = -mysign((Sig_11-Sig_33))*np.sqrt(0.5*(1.-cos2theta))

        Sig_11_hat = 0.5*(W+signR*sqrtT);
        Sig_33_hat = 0.5*(W-signR*sqrtT);

        dS_cos2theta = signR*(dS_R/sqrtT - R/(2*sqrtT*sqrtT*sqrtT)*dS_T);
        dS_costheta = 1./(4.*costheta)*dS_cos2theta;

        if (fabs(sintheta)<threshold_singular && handle_singularities){
        //equivalent to to np.abs(Sig_13)<threshold_singular
            dS_sintheta = (Sig_14+Sig_23)/R;
        }
        else{
            dS_sintheta = -1./(4.*sintheta)*dS_cos2theta;
        }

        dS_Sig_11_hat = 0.5*(dS_W + signR*0.5/sqrtT*dS_T);
        dS_Sig_33_hat = 0.5*(dS_W - signR*0.5/sqrtT*dS_T);
    }

    *Sig_11_hat_ptr = Sig_11_hat;
    *Sig_33_hat_ptr = Sig_33_hat;
    *costheta_ptr = costheta;
    *sintheta_ptr = sintheta;
    *dS_Sig_11_hat_ptr = dS_Sig_11_hat;
    *dS_Sig_33_hat_ptr = dS_Sig_33_hat;
    *dS_costheta_ptr = dS_costheta;
    *dS_sintheta_ptr = dS_sintheta;

}

/*gpufun*/
void BoostParameters_boost_coordinates(
    const BoostParameters bp,
    double* x_star,
    double* px_star,
    double* y_star,
    double* py_star,
    double* sigma_star,
    double* delta_star){

    double const sphi = BoostParameters_get_sphi(bp);
    double const cphi = BoostParameters_get_cphi(bp);
    double const tphi = BoostParameters_get_tphi(bp);
    double const salpha = BoostParameters_get_salpha(bp);
    double const calpha = BoostParameters_get_calpha(bp);

    double const x = *x_star;
    double const px = *px_star;
    double const y = *y_star;
    double const py = *py_star ;
    double const sigma = *sigma_star;
    double const delta = *delta_star ;

    double const h = delta + 1. - sqrt((1.+delta)*(1.+delta)-px*px-py*py);


    double const px_st = px/cphi-h*calpha*tphi/cphi;
    double const py_st = py/cphi-h*salpha*tphi/cphi;
    double const delta_st = delta -px*calpha*tphi-py*salpha*tphi+h*tphi*tphi;

    double const pz_st =
        sqrt((1.+delta_st)*(1.+delta_st)-px_st*px_st-py_st*py_st);

    double const hx_st = px_st/pz_st;
    double const hy_st = py_st/pz_st;
    double const hsigma_st = 1.-(delta_st+1)/pz_st;

    double const L11 = 1.+hx_st*calpha*sphi;
    double const L12 = hx_st*salpha*sphi;
    double const L13 = calpha*tphi;

    double const L21 = hy_st*calpha*sphi;
    double const L22 = 1.+hy_st*salpha*sphi;
    double const L23 = salpha*tphi;

    double const L31 = hsigma_st*calpha*sphi;
    double const L32 = hsigma_st*salpha*sphi;
    double const L33 = 1./cphi;

    double const x_st = L11*x + L12*y + L13*sigma;
    double const y_st = L21*x + L22*y + L23*sigma;
    double const sigma_st = L31*x + L32*y + L33*sigma;

    *x_star = x_st;
    *px_star = px_st;
    *y_star = y_st;
    *py_star = py_st;
    *sigma_star = sigma_st;
    *delta_star = delta_st;

}

/*gpufun*/
void BoostParameters_boost_coordinates(
    	const BoostParameters bp,
    	double* x,
    	double* px,
    	double* y,
    	double* py,
    	double* sigma,
    	double* delta){

    double const sphi = BoostParameters_get_sphi(bp);
    double const cphi = BoostParameters_get_cphi(bp);
    double const tphi = BoostParameters_get_tphi(bp);
    double const salpha = BoostParameters_get_salpha(bp);
    double const calpha = BoostParameters_get_calpha(bp);

    double const x_st = *x;
    double const px_st = *px;
    double const y_st = *y;
    double const py_st = *py ;
    double const sigma_st = *sigma;
    double const delta_st = *delta ;

    double const pz_st = sqrt((1.+delta_st)*(1.+delta_st)-px_st*px_st-py_st*py_st);
    double const hx_st = px_st/pz_st;
    double const hy_st = py_st/pz_st;
    double const hsigma_st = 1.-(delta_st+1)/pz_st;

    double const Det_L =
        1./cphi + (hx_st*calpha + hy_st*salpha-hsigma_st*sphi)*tphi;

    double const Linv_11 =
        (1./cphi + salpha*tphi*(hy_st-hsigma_st*salpha*sphi))/Det_L;

    double const Linv_12 =
        (salpha*tphi*(hsigma_st*calpha*sphi-hx_st))/Det_L;

    double const Linv_13 =
        -tphi*(calpha - hx_st*salpha*salpha*sphi + hy_st*calpha*salpha*sphi)/Det_L;

    double const Linv_21 =
        (calpha*tphi*(-hy_st + hsigma_st*salpha*sphi))/Det_L;

    double const Linv_22 =
        (1./cphi + calpha*tphi*(hx_st-hsigma_st*calpha*sphi))/Det_L;

    double const Linv_23 =
        -tphi*(salpha - hy_st*calpha*calpha*sphi + hx_st*calpha*salpha*sphi)/Det_L;

    double const Linv_31 = -hsigma_st*calpha*sphi/Det_L;
    double const Linv_32 = -hsigma_st*salpha*sphi/Det_L;
    double const Linv_33 = (1. + hx_st*calpha*sphi + hy_st*salpha*sphi)/Det_L;

    double const x_i = Linv_11*x_st + Linv_12*y_st + Linv_13*sigma_st;
    double const y_i = Linv_21*x_st + Linv_22*y_st + Linv_23*sigma_st;
    double const sigma_i = Linv_31*x_st + Linv_32*y_st + Linv_33*sigma_st;

    double const h = (delta_st+1.-pz_st)*cphi*cphi;

    double const px_i = px_st*cphi+h*calpha*tphi;
    double const py_i = py_st*cphi+h*salpha*tphi;

    double const delta_i = delta_st + px_i*calpha*tphi + py_i*salpha*tphi - h*tphi*tphi;


    *x = x_i;
    *px = px_i;
    *y = y_i;
    *py = py_i;
    *sigma = sigma_i;
    *delta = delta_i;

}

#endif
