/**
	\file cFragmentIons.h
	\brief Basic definitions of fragment ions occurring in the mass spectra.
*/


#ifndef _CFRAGMENTIONS_H
#define _CFRAGMENTIONS_H

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <vector>
#include <QMetaType>
#include "core/utilities.h"


using namespace std;


// monoisotopic masses
const double H = 1.0078250321;
const double D = 2.014102;
const double T = 3.016049;
const double He = 4.002606;
const double Li = 7.016004;
const double Be = 9.012182;
const double B = 11.009306;
const double C = 12.0;
const double N = 14.0030740052;
const double O = 15.9949146221;
const double F = 18.998404;
const double Ne = 19.992439;
const double Na = 22.989771;
const double Mg = 23.985043;
const double Al = 26.981539;
const double Si = 27.976927;
const double P = 30.97376151;
const double S = 31.97207069;
const double Cl = 34.968853;
const double Ar = 39.962383;
const double K = 38.963707;
const double Ca = 39.962589;
const double Sc = 44.95591;
const double Ti = 47.947948;
const double V = 50.943962;
const double Cr = 51.940514;
const double Mn = 54.938049;
const double Fe = 55.93494;
const double Co = 58.933201;
const double Ni = 57.935349;
const double Cu = 62.9296;
const double Zn = 63.929146;
const double Ga = 68.925583;
const double Ge = 73.921181;
const double As = 74.921600;
const double Se = 79.916519;
const double Br = 78.918327;
const double Kr = 83.911507;
const double Rb = 84.911789;
const double Sr = 87.905617;
const double Y = 88.905846;
const double Zr = 89.904701;
const double Nb = 92.906403;
const double Mo = 97.905411;
const double Tc = 97.907219;
const double Ru = 101.904388;
const double Rh = 102.905502;
const double Pd = 105.903481;
const double Ag = 106.905090;
const double Cd = 113.903358;
const double In = 114.903877;
const double Sn = 119.902199;
const double Sb = 120.903824;
const double Te = 129.906219;
const double I = 126.904457;
const double Xe = 131.904160;
const double Cs = 132.905441;
const double Ba = 137.905243;
const double La = 138.906342;
const double Ce = 139.905441;
const double Pr = 140.907593;
const double Nd = 141.907700;
const double Pm = 144.912750;
const double Sm = 151.919693;
const double Eu = 152.921204;
const double Gd = 157.924103;
const double Tb = 158.925293;
const double Dy = 163.929199;
const double Ho = 164.930298;
const double Er = 165.930298;
const double Tm = 168.934204;
const double Yb = 173.938904;
const double Lu = 174.940796;
const double Hf = 179.946503;
const double Ta = 180.947998;
const double W = 183.950897;
const double Re = 186.955597;
const double Os = 191.961502;
const double Ir = 192.962906;
const double Pt = 194.964798;
const double Au = 196.966507;
const double Hg = 201.970596;
const double Tl = 204.974396;
const double Pb = 207.976593;
const double Bi = 208.980392;
const double Po = 208.982422;
const double At = 209.987137;
const double Rn = 222.017563;
const double Fr = 209.996399;
const double Ra = 226.025406;
const double Ac = 227.027740;
const double Th = 232.038055;
const double Pa = 231.035904;
const double U = 238.050781;
const double Np = 237.048172;
const double Pu = 242.058746;
const double Am = 243.061371;
const double Cm = 247.070343;
const double Bk = 247.070297;
const double Cf = 251.079575;
const double Es = 252.082977;
const double Fm = 257.095093;
const double Md = 258.098419;
const double No = 259.101044;
const double Lr = 262.109802;

const double R = 0;
const double X = 0;
const double e = H - 1.00727645;


// monoisotopic masses of natural isotopes
const double H1 = H;
const double H2 = D;
//const double H3 = T;
const double He3 = 3.016029;
const double He4 = He;
const double Li6 = 6.015122;
const double Li7 = Li;
const double Be9 = Be;
const double B10 = 10.012937;
const double B11 = B;
const double C12 = C;
const double C13 = 13.003355;
//const double C14 = 14.003242;
const double N14 = N;
const double N15 = 15.000109;
const double O16 = O;
const double O17 = 16.999132;
const double O18 = 17.999160;
const double F19 = F;
const double Ne20 = Ne;
const double Ne21 = 20.993847;
const double Ne22 = 21.991386;
const double Na23 = Na;
const double Mg24 = Mg;
const double Mg25 = 24.985837;
const double Mg26 = 25.982593;
const double Al27 = Al;
const double Si28 = Si;
const double Si29 = 28.97649;
const double Si30 = 29.973770;
const double P31 = P;
const double S32 = S;
const double S33 = 32.971458;
const double S34 = 33.967867;
const double S36 = 35.967081;
const double Cl35 = Cl;
const double Cl37 = 36.965903;
const double Ar36 = 35.967546;
const double Ar38 = 37.962732;
const double Ar40 = Ar;
const double K39 = K;
const double K40 = 39.963999;
const double K41 = 40.961825;
const double Ca40 = Ca;
const double Ca42 = 41.958618;
const double Ca43 = 42.958767;
const double Ca44 = 43.955481;
const double Ca46 = 45.953693;
const double Ca48 = 47.952534;
const double Sc45 = Sc;
const double Ti46 = 45.952629;
const double Ti47 = 46.951764;
const double Ti48 = Ti;
const double Ti49 = 48.947871;
const double Ti50 = 49.944792;
const double V50 = 49.947163;
const double V51 = V;
const double Cr50 = 49.946046;
const double Cr52 = Cr;
const double Cr53 = 52.940651;
const double Cr54 = 53.938885;
const double Mn55 = Mn;
const double Fe54 = 53.939612;
const double Fe56 = Fe;
const double Fe57 = 56.935399;
const double Fe58 = 57.933280;
const double Co59 = Co;
const double Ni58 = Ni;
const double Ni60 = 59.930788;
const double Ni61 = 60.931060;
const double Ni62 = 61.928346;
const double Ni64 = 63.927970;
const double Cu63 = Cu;
const double Cu65 = 64.927793;
const double Zn64 = Zn;
const double Zn66 = 65.926034;
const double Zn67 = 66.927129;
const double Zn68 = 67.924846;
const double Zn70 = 69.925325;
const double Ga69 = Ga;
const double Ga71 = 70.9247;
const double Ge70 = 69.924250;
const double Ge72 = 71.922076;
const double Ge73 = 72.923459;
const double Ge74 = Ge;
const double Ge76 = 75.921403;
const double As75 = As;
const double Se74 = 73.922477;
const double Se76 = 75.919214;
const double Se77 = 76.919915;
const double Se78 = 77.917310;
const double Se80 = Se;
const double Se82 = 81.9167;
const double Br79 = Br;
const double Br81 = 80.916291;
const double Kr78 = 77.920386;
const double Kr80 = 79.916378;
const double Kr82 = 81.913485;
const double Kr83 = 82.914136;
const double Kr84 = Kr;
const double Kr86 = 85.910610;
const double Rb85 = Rb;
const double Rb87 = 86.909183;
const double Sr84 = 83.913425;
const double Sr86 = 85.909262;
const double Sr87 = 86.908879;
const double Sr88 = Sr;
const double Y89 = Y;
const double Zr90 = Zr;
const double Zr91 = 90.905645;
const double Zr92 = 91.90504;
const double Zr94 = 93.906316;
const double Zr96 = 95.908276;
const double Nb93 = Nb;
const double Mo92 = 91.906810;
const double Mo94 = 93.905088;
const double Mo95 = 94.905841;
const double Mo96 = 95.904679;
const double Mo97 = 96.906021;
const double Mo98 = Mo;
const double Mo100 = 99.907477;
const double Tc98 = Tc;
const double Ru96 = 95.907598;
const double Ru98 = 97.905287;
const double Ru99 = 98.905939;
const double Ru100 = 99.904220;
const double Ru101 = 100.905582;
const double Ru102 = Ru;
const double Ru104 = 103.90543;
const double Rh103 = Rh;
const double Pd102 = 101.905608;
const double Pd104 = 103.904035;
const double Pd105 = 104.905084;
const double Pd106 = Pd;
const double Pd108 = 107.903894;
const double Pd110 = 109.905152;
const double Ag107 = Ag;
const double Ag109 = 108.904756;
const double Cd106 = 105.906458;
const double Cd108 = 107.904183;
const double Cd110 = 109.903006;
const double Cd111 = 110.904182;
const double Cd112 = 111.902757;
const double Cd113 = 112.904401;
const double Cd114 = Cd;
const double Cd116 = 115.904755;
const double In113 = 112.904061;
const double In115 = In;
const double Sn112 = 111.904821;
const double Sn114 = 113.902782;
const double Sn115 = 114.903346;
const double Sn116 = 115.901744;
const double Sn117 = 116.902954;
const double Sn118 = 117.901606;
const double Sn119 = 118.903309;
const double Sn120 = Sn;
const double Sn122 = 121.90344;
const double Sn124 = 123.905275;
const double Sb121 = Sb;
const double Sb123 = 122.904216;
const double Te120 = 119.90402;
const double Te122 = 121.903047;
const double Te123 = 122.904273;
const double Te124 = 123.902819;
const double Te125 = 124.904425;
const double Te126 = 125.903306;
const double Te128 = 127.904461;
const double Te130 = Te;
const double I127 = I;
const double Xe124 = 123.905896;
const double Xe126 = 125.904269;
const double Xe128 = 127.90353;
const double Xe129 = 128.904779;
const double Xe130 = 129.903508;
const double Xe131 = 130.905082;
const double Xe132 = Xe;
const double Xe134 = 133.905395;
const double Xe136 = 135.90722;
const double Cs133 = Cs;
const double Ba130 = 129.906310;
const double Ba132 = 131.905056;
const double Ba134 = 133.904503;
const double Ba135 = 134.905683;
const double Ba136 = 135.90457;
const double Ba137 = 136.905821;
const double Ba138 = Ba;
const double La138 = 137.907107;
const double La139 = La;
const double Ce136 = 135.907144;
const double Ce138 = 137.905986;
const double Ce140 = Ce;
const double Ce142 = 141.90924;
const double Pr141 = Pr;
const double Nd142 = Nd;
const double Nd143 = 142.90981;
const double Nd144 = 143.910083;
const double Nd145 = 144.912569;
const double Nd146 = 145.913112;
const double Nd148 = 147.916889;
const double Nd150 = 149.920887;
const double Pm145 = Pm;
const double Sm144 = 143.911995;
const double Sm147 = 146.914893;
const double Sm148 = 147.914818;
const double Sm149 = 148.917180;
const double Sm150 = 149.917271;
const double Sm152 = Sm;
const double Sm154 = 153.922205;
const double Eu151 = 150.919846;
const double Eu153 = Eu;
const double Gd152 = 151.919788;
const double Gd154 = 153.920862;
const double Gd155 = 154.922619;
const double Gd156 = 155.92212;
const double Gd157 = 156.923957;
const double Gd158 = Gd;
const double Gd160 = 159.927051;
const double Tb159 = Tb;
const double Dy156 = 155.924278;
const double Dy158 = 157.924405;
const double Dy160 = 159.925194;
const double Dy161 = 160.926930;
const double Dy162 = 161.926795;
const double Dy163 = 162.928728;
const double Dy164 = Dy;
const double Ho165 = Ho;
const double Er162 = 161.928775;
const double Er164 = 163.929197;
const double Er166 = Er;
const double Er167 = 166.932045;
const double Er168 = 167.932368;
const double Er170 = 169.935460;
const double Tm169 = Tm;
const double Yb168 = 167.933894;
const double Yb170 = 169.934759;
const double Yb171 = 170.936322;
const double Yb172 = 171.936378;
const double Yb173 = 172.938207;
const double Yb174 = Yb;
const double Yb176 = 175.942568;
const double Lu175 = Lu;
const double Lu176 = 175.942682;
const double Hf174 = 173.94004;
const double Hf176 = 175.941402;
const double Hf177 = 176.94322;
const double Hf178 = 177.943698;
const double Hf179 = 178.945815;
const double Hf180 = Hf;
const double Ta180 = 179.947466;
const double Ta181 = Ta;
const double W180 = 179.946706;
const double W182 = 181.948206;
const double W183 = 182.950224;
const double W184 = W;
const double W186 = 185.954362;
const double Re185 = 184.952956;
const double Re187 = Re;
const double Os184 = 183.952491;
const double Os186 = 185.953838;
const double Os187 = 186.955748;
const double Os188 = 187.955836;
const double Os189 = 188.958145;
const double Os190 = 189.958445;
const double Os192 = Os;
const double Ir191 = 190.960591;
const double Ir193 = Ir;
const double Pt190 = 189.95993;
const double Pt192 = 191.961035;
const double Pt194 = 193.962664;
const double Pt195 = Pt;
const double Pt196 = 195.964935;
const double Pt198 = 197.967876;
const double Au197 = Au;
const double Hg196 = 195.965815;
const double Hg198 = 197.966752;
const double Hg199 = 198.968262;
const double Hg200 = 199.968309;
const double Hg201 = 200.970285;
const double Hg202 = Hg;
const double Hg204 = 203.973476;
const double Tl203 = 202.972329;
const double Tl205 = Tl;
const double Pb204 = 203.973029;
const double Pb206 = 205.974449;
const double Pb207 = 206.975881;
const double Pb208 = Pb;
const double Bi209 = Bi;
const double Po209 = Po;
const double At210 = At;
const double Rn222 = Rn;
const double Fr223 = Fr;
const double Ra226 = Ra;
const double Ac227 = Ac;
const double Th232 = Th;
const double Pa231 = Pa;
const double U234 = 234.040946;
const double U235 = 235.043923;
const double U238 = U;
const double Np237 = Np;
const double Pu244 = Pu;
const double Am243 = Am;
const double Cm247 = Cm;
const double Bk247 = Bk;
const double Cf251 = Cf;
const double Es252 = Es;
const double Fm257 = Fm;
const double Md258 = Md;
const double No259 = No;
const double Lr262 = Lr;


// probabilities of natural isotope abundances
const double Pr_H1 = 0.999885;
const double Pr_H2 = 0.000115;
const double Pr_He3 = 0.00000137;
const double Pr_He4 = 0.99999863;
const double Pr_Li6 = 0.0759;
const double Pr_Li7 = 0.9241;
const double Pr_Be9 = 1;
const double Pr_B10 = 0.00199;
const double Pr_B11 = 0.00801;
const double Pr_C12 = 0.9893;
const double Pr_C13 = 0.0107;
const double Pr_N14 = 0.99632;
const double Pr_N15 = 0.00368;
const double Pr_O16 = 0.99757;
const double Pr_O17 = 0.00038;
const double Pr_O18 = 0.00205;
const double Pr_F19 = 1;
const double Pr_Ne20 = 0.9048;
const double Pr_Ne21 = 0.0027;
const double Pr_Ne22 = 0.0925;
const double Pr_Na23 = 1;
const double Pr_Mg24 = 0.7899;
const double Pr_Mg25 = 0.1;
const double Pr_Mg26 = 0.1101;
const double Pr_Al27 = 1;
const double Pr_Si28 = 0.922297;
const double Pr_Si29 = 0.046832;
const double Pr_Si30 = 0.030872;
const double Pr_P31 = 1;
const double Pr_S32 = 0.9493;
const double Pr_S33 = 0.0076;
const double Pr_S34 = 0.0429;
const double Pr_S36 = 0.0002;
const double Pr_Cl35 = 0.7578;
const double Pr_Cl37 = 0.2422;
const double Pr_Ar36 = 0.003365;
const double Pr_Ar38 = 0.000632;
const double Pr_Ar40 = 0.996003;
const double Pr_K39 = 0.932581;
const double Pr_K40 = 0.000117;
const double Pr_K41 = 0.067302;
const double Pr_Ca40 = 0.96941;
const double Pr_Ca42 = 0.00647;
const double Pr_Ca43 = 0.00135;
const double Pr_Ca44 = 0.02086;
const double Pr_Ca46 = 0.00004;
const double Pr_Ca48 = 0.00187;
const double Pr_Sc45 = 1;
const double Pr_Ti46 = 0.0825;
const double Pr_Ti47 = 0.0744;
const double Pr_Ti48 = 0.7372;
const double Pr_Ti49 = 0.0541;
const double Pr_Ti50 = 0.0518;
const double Pr_V50 = 0.0025;
const double Pr_V51 = 0.9975;
const double Pr_Cr50 = 0.04345;
const double Pr_Cr52 = 0.83789;
const double Pr_Cr53 = 0.09501;
const double Pr_Cr54 = 0.02365;
const double Pr_Mn55 = 1;
const double Pr_Fe54 = 0.05845;
const double Pr_Fe56 = 0.91754;
const double Pr_Fe57 = 0.02119;
const double Pr_Fe58 = 0.00282;
const double Pr_Co59 = 1;
const double Pr_Ni58 = 0.680769;
const double Pr_Ni60 = 0.262231;
const double Pr_Ni61 = 0.011399;
const double Pr_Ni62 = 0.036345;
const double Pr_Ni64 = 0.009256;
const double Pr_Cu63 = 0.6917;
const double Pr_Cu65 = 0.3083;
const double Pr_Zn64 = 0.4863;
const double Pr_Zn66 = 0.2790;
const double Pr_Zn67 = 0.0410;
const double Pr_Zn68 = 0.1875;
const double Pr_Zn70 = 0.0062;
const double Pr_Ga69 = 0.60108;
const double Pr_Ga71 = 0.39892;
const double Pr_Ge70 = 0.2084;
const double Pr_Ge72 = 0.2754;
const double Pr_Ge73 = 0.0773;
const double Pr_Ge74 = 0.3628;
const double Pr_Ge76 = 0.0761;
const double Pr_As75 = 1;
const double Pr_Se74 = 0.0089;
const double Pr_Se76 = 0.0937;
const double Pr_Se77 = 0.0763;
const double Pr_Se78 = 0.2377;
const double Pr_Se80 = 0.4961;
const double Pr_Se82 = 0.0873;
const double Pr_Br79 = 0.5069;
const double Pr_Br81 = 0.4931;
const double Pr_Kr78 = 0.0035;
const double Pr_Kr80 = 0.0228;
const double Pr_Kr82 = 0.1158;
const double Pr_Kr83 = 0.1149;
const double Pr_Kr84 = 0.57;
const double Pr_Kr86 = 0.173;
const double Pr_Rb85 = 0.7217;
const double Pr_Rb87 = 0.2783;
const double Pr_Sr84 = 0.0056;
const double Pr_Sr86 = 0.0986;
const double Pr_Sr87 = 0.07;
const double Pr_Sr88 = 0.8258;
const double Pr_Y89 = 1;
const double Pr_Zr90 = 0.5145;
const double Pr_Zr91 = 0.1122;
const double Pr_Zr92 = 0.1715;
const double Pr_Zr94 = 0.1738;
const double Pr_Zr96 = 0.028;
const double Pr_Nb93 = 1;
const double Pr_Mo92 = 0.1484;
const double Pr_Mo94 = 0.0925;
const double Pr_Mo95 = 0.1592;
const double Pr_Mo96 = 0.1668;
const double Pr_Mo97 = 0.0955;
const double Pr_Mo98 = 0.2413;
const double Pr_Mo100 = 0.0963;
const double Pr_Tc98 = 1;
const double Pr_Ru96 = 0.0554;
const double Pr_Ru98 = 0.0187;
const double Pr_Ru99 = 0.1276;
const double Pr_Ru100 = 0.126;
const double Pr_Ru101 = 0.1706;
const double Pr_Ru102 = 0.3155;
const double Pr_Ru104 = 0.1862;
const double Pr_Rh103 = 1;
const double Pr_Pd102 = 0.0102;
const double Pr_Pd104 = 0.1114;
const double Pr_Pd105 = 0.2233;
const double Pr_Pd106 = 0.2733;
const double Pr_Pd108 = 0.2646;
const double Pr_Pd110 = 0.1172;
const double Pr_Ag107 = 0.51839;
const double Pr_Ag109 = 0.48161;
const double Pr_Cd106 = 0.0125;
const double Pr_Cd108 = 0.0089;
const double Pr_Cd110 = 0.1249;
const double Pr_Cd111 = 0.128;
const double Pr_Cd112 = 0.2413;
const double Pr_Cd113 = 0.1222;
const double Pr_Cd114 = 0.2873;
const double Pr_Cd116 = 0.0749;
const double Pr_In113 = 0.0429;
const double Pr_In115 = 0.9571;
const double Pr_Sn112 = 0.0097;
const double Pr_Sn114 = 0.0066;
const double Pr_Sn115 = 0.0034;
const double Pr_Sn116 = 0.1454;
const double Pr_Sn117 = 0.0768;
const double Pr_Sn118 = 0.2422;
const double Pr_Sn119 = 0.0859;
const double Pr_Sn120 = 0.3258;
const double Pr_Sn122 = 0.0463;
const double Pr_Sn124 = 0.0579;
const double Pr_Sb121 = 0.5721;
const double Pr_Sb123 = 0.4279;
const double Pr_Te120 = 0.0009;
const double Pr_Te122 = 0.0255;
const double Pr_Te123 = 0.0089;
const double Pr_Te124 = 0.0474;
const double Pr_Te125 = 0.0707;
const double Pr_Te126 = 0.1884;
const double Pr_Te128 = 0.3174;
const double Pr_Te130 = 0.3408;
const double Pr_I127 = 1;
const double Pr_Xe124 = 0.0009;
const double Pr_Xe126 = 0.0009;
const double Pr_Xe128 = 0.0192;
const double Pr_Xe129 = 0.2644;
const double Pr_Xe130 = 0.0408;
const double Pr_Xe131 = 0.2118;
const double Pr_Xe132 = 0.2689;
const double Pr_Xe134 = 0.1044;
const double Pr_Xe136 = 0.0887;
const double Pr_Cs133 = 1;
const double Pr_Ba130 = 0.00106;
const double Pr_Ba132 = 0.00101;
const double Pr_Ba134 = 0.02417;
const double Pr_Ba135 = 0.06592;
const double Pr_Ba136 = 0.07854;
const double Pr_Ba137 = 0.11232;
const double Pr_Ba138 = 0.71698;
const double Pr_La138 = 0.0009;
const double Pr_La139 = 0.9991;
const double Pr_Ce136 = 0.00185;
const double Pr_Ce138 = 0.00251;
const double Pr_Ce140 = 0.8845;
const double Pr_Ce142 = 0.11114;
const double Pr_Pr141 = 1;
const double Pr_Nd142 = 0.272;
const double Pr_Nd143 = 0.122;
const double Pr_Nd144 = 0.238;
const double Pr_Nd145 = 0.083;
const double Pr_Nd146 = 0.172;
const double Pr_Nd148 = 0.057;
const double Pr_Nd150 = 0.056;
const double Pr_Pm145 = 1;
const double Pr_Sm144 = 0.0307;
const double Pr_Sm147 = 0.1499;
const double Pr_Sm148 = 0.1124;
const double Pr_Sm149 = 0.1382;
const double Pr_Sm150 = 0.0738;
const double Pr_Sm152 = 0.2675;
const double Pr_Sm154 = 0.2275;
const double Pr_Eu151 = 0.4781;
const double Pr_Eu153 = 0.5219;
const double Pr_Gd152 = 0.002;
const double Pr_Gd154 = 0.0218;
const double Pr_Gd155 = 0.148;
const double Pr_Gd156 = 0.2047;
const double Pr_Gd157 = 0.1565;
const double Pr_Gd158 = 0.2484;
const double Pr_Gd160 = 0.2186;
const double Pr_Tb159 = 1;
const double Pr_Dy156 = 0.0006;
const double Pr_Dy158 = 0.001;
const double Pr_Dy160 = 0.0234;
const double Pr_Dy161 = 0.1891;
const double Pr_Dy162 = 0.2551;
const double Pr_Dy163 = 0.249;
const double Pr_Dy164 = 0.2818;
const double Pr_Ho165 = 1;
const double Pr_Er162 = 0.0014;
const double Pr_Er164 = 0.0161;
const double Pr_Er166 = 0.3361;
const double Pr_Er167 = 0.2293;
const double Pr_Er168 = 0.2678;
const double Pr_Er170 = 0.1493;
const double Pr_Tm169 = 1;
const double Pr_Yb168 = 0.0013;
const double Pr_Yb170 = 0.0304;
const double Pr_Yb171 = 0.1428;
const double Pr_Yb172 = 0.2183;
const double Pr_Yb173 = 0.1613;
const double Pr_Yb174 = 0.3183;
const double Pr_Yb176 = 0.1276;
const double Pr_Lu175 = 0.9741;
const double Pr_Lu176 = 0.0259;
const double Pr_Hf174 = 0.0016;
const double Pr_Hf176 = 0.0526;
const double Pr_Hf177 = 0.186;
const double Pr_Hf178 = 0.2728;
const double Pr_Hf179 = 0.1362;
const double Pr_Hf180 = 0.3508;
const double Pr_Ta180 = 0.00012;
const double Pr_Ta181 = 0.99988;
const double Pr_W180 = 0.0012;
const double Pr_W182 = 0.265;
const double Pr_W183 = 0.1431;
const double Pr_W184 = 0.3064;
const double Pr_W186 = 0.2843;
const double Pr_Re185 = 0.374;
const double Pr_Re187 = 0.626;
const double Pr_Os184 = 0.0002;
const double Pr_Os186 = 0.0159;
const double Pr_Os187 = 0.0196;
const double Pr_Os188 = 0.1324;
const double Pr_Os189 = 0.1615;
const double Pr_Os190 = 0.2626;
const double Pr_Os192 = 0.4078;
const double Pr_Ir191 = 0.373;
const double Pr_Ir193 = 0.627;
const double Pr_Pt190 = 0.00014;
const double Pr_Pt192 = 0.00782;
const double Pr_Pt194 = 0.32967;
const double Pr_Pt195 = 0.33832;
const double Pr_Pt196 = 0.25242;
const double Pr_Pt198 = 0.07163;
const double Pr_Au197 = 1;
const double Pr_Hg196 = 0.0015;
const double Pr_Hg198 = 0.0997;
const double Pr_Hg199 = 0.1687;
const double Pr_Hg200 = 0.231;
const double Pr_Hg201 = 0.1318;
const double Pr_Hg202 = 0.2986;
const double Pr_Hg204 = 0.0687;
const double Pr_Tl203 = 0.29524;
const double Pr_Tl205 = 0.70476;
const double Pr_Pb204 = 0.014;
const double Pr_Pb206 = 0.241;
const double Pr_Pb207 = 0.221;
const double Pr_Pb208 = 0.524;
const double Pr_Bi209 = 1;
const double Pr_Po209 = 1;
const double Pr_At210 = 1;
const double Pr_Rn222 = 1;
const double Pr_Fr223 = 1;
const double Pr_Ra226 = 1;
const double Pr_Ac227 = 1;
const double Pr_Th232 = 1;
const double Pr_Pa231 = 1;
const double Pr_U234 = 0.000055;
const double Pr_U235 = 0.0072;
const double Pr_U238 = 0.992745;
const double Pr_Np237 = 1;
const double Pr_Pu244 = 1;
const double Pr_Am243 = 1;
const double Pr_Cm247 = 1;
const double Pr_Bk247 = 1;
const double Pr_Cf251 = 1;
const double Pr_Es252 = 1;
const double Pr_Fm257 = 1;
const double Pr_Md258 = 1;
const double Pr_No259 = 1;
const double Pr_Lr262 = 1;


/**
	\brief The class representing a periodic table.
*/
class cPeriodicTableMap {

	map<string, double> table;
	vector<string> adductElements;

public:
	

	/**
		\brief The constructor.
	*/ 
	cPeriodicTableMap();


	/**
		\brief Overloaded operator [].
		\param element reference to a string with a name of an element
		\retval double mass of an element
	*/ 
	double &operator[](string& element);


	/**
		\brief Check whether an element occurs in the table.
		\param element reference to a string with a name of an element
		\retval int 1 when element is presented in the table, 0 otherwise
	*/ 
	int count(string& element);


	/**
		\brief Reference to the map of elements.
		\retval map<string, double> map of elements
	*/ 
	map<string, double>& getElements();


	/**
		\brief Vector of elements which are allowed to be adducts.
		\retval vector<string> adducts
	*/ 
	vector<string>& getAdductElements();

};


/**
	\brief Global periodic table map.
*/
extern cPeriodicTableMap periodictablemap;


/**
	\brief Representation of an isotope.
*/
struct isotope {

	/**
		\brief Monoisotopic mass.
	*/ 
	double mass;


	/**
		\brief Probability of natural abundance.
	*/ 
	double probability;


	/**
		\brief The default constructor.
	*/ 
	isotope() {
		clear();
	}


	/**
		\brief Clear the structure.
	*/ 
	void clear() {
		mass = 0;
		probability = 0;
	}

};


/**
	\brief The class representing an isotope map.
*/
class cIsotopeMap {

	map< string, map<string, isotope> > table;

public:
	

	/**
		\brief The constructor.
	*/ 
	cIsotopeMap();


	/**
		\brief Overloaded operator [].
		\param element reference to a string with a name of an element
		\retval map map of isotopes of the element
	*/ 
	map<string, isotope> &operator[](string& element);


	/**
		\brief Check whether an element occurs in the table.
		\param element reference to a string with a name of an element
		\retval int 1 when element is presented in the table, 0 otherwise
	*/ 
	int count(string& element);

};


/**
	\brief Global isotope map.
*/
extern cIsotopeMap isotopemap;


const double H2O = 2 * H + O;
const double NH3 = 3 * H + N;
const double CO = C + O;

const double A_ION = H - e - C - O;
const double B_ION = H - e;
const double C_ION = H - e + NH3;
const double X_ION = H - e + 2 * O + C;
const double Y_ION = H - e + H2O;
const double Z_ION = H - e + O - N - H;

const double PRECURSOR_ION = H - e + H2O;
const double PRECURSOR_ION_CYCLIC = H - e;

const double L0H_ION = H - e - 2 * H;
const double L1H_ION = H - e;
const double L2H_ION = H - e + 2 * H;
const double R1H_ION = H - e;
const double R2H_ION = H - e + 2 * H;
const double L1OH_ION = H - e + O;
const double L2OH_ION = H - e + H2O;
const double R1OH_ION = H - e + O;
const double R2OH_ION = H - e + H2O;

const double PRECURSOR_ION_LINEAR_POLYKETIDE_H_H = H - e + 2 * H;
const double PRECURSOR_ION_LINEAR_POLYKETIDE_H_OH = H - e + H2O;
const double PRECURSOR_ION_LINEAR_POLYKETIDE_OH_OH = H - e + 2 * H + 2 * O;


/**
	\brief Register ePeptideType by Qt.
*/
Q_DECLARE_METATYPE(ePeptideType);


/**
	\brief Get mzratio from neutral mass.
	\param neutralmass neutral mass
	\param charge charge
	\retval double mzratio
*/ 
double charge(double neutralmass, int charge);


/**
	\brief Get neutral mass from mzratio.
	\param mzratio mzratio
	\param charge charge
	\retval double neutral mass
*/ 
double uncharge(double mzratio, int charge);


/**
	\brief The types of supported fragment ions.
*/
enum eFragmentIonType {
	a_ion = 1,
	b_ion,
	c_ion,
	x_ion,
	y_ion,
	z_ion,
	precursor_ion,
	cyclic_precursor_ion,
	l0h_ion,
	l1h_ion,
	l2h_ion,
	r1h_ion,
	r2h_ion,
	l1oh_ion,
	l2oh_ion,
	r1oh_ion,
	r2oh_ion,
	linear_polyketide_precursor_ion_h_h,
	linear_polyketide_precursor_ion_h_oh,
	linear_polyketide_precursor_ion_oh_oh,
	cyclic_polyketide_precursor_ion,
	ms_Hplus,
	ms_Naplus,
	ms_Kplus,
	ms_Hminus,
	ms_Naminus,
	ms_Kminus,
	ms_MFe2H,
	ms_MFe3HNa,
	ms_MFe3HK,
	ms_2MFe2H,
	ms_2MFe3HNa,
	ms_2MFe3HK,
	ms_3MFe2H,
	ms_3MFe3HNa,
	ms_3MFe3HK,
	ms_3M2Fe5H,
	ms_3M2Fe6HNa,
	ms_3M2Fe6HK,
	ms_MFe4H,
	ms_2MFe4H,
	ms_3MFe4H,
	ms_3M2Fe7H,
	ms_NH4plus,
	ms_Clminus,
	ms_plus,
	ms_minus,
	ms_liplus,
	ms_MMgH,
	ms_MMg2HNa,
	ms_MMg2HK,
	ms_MMg3H,
	ms_MAl2H,
	ms_MAl3HNa,
	ms_MAl3HK,
	ms_MAl4H,
	ms_MSi3H,
	ms_MSi4HNa,
	ms_MSi4HK,
	ms_MSi5H,
	ms_MCaH,
	ms_MCa2HNa,
	ms_MCa2HK,
	ms_MCa3H,
	ms_MSc2H,
	ms_MSc3HNa,
	ms_MSc3HK,
	ms_MSc4H,
	ms_MCr2H,
	ms_MCr3HNa,
	ms_MCr3HK,
	ms_MCr4H,
	ms_MMnH,
	ms_MMn2HNa,
	ms_MMn2HK,
	ms_MMn3H,
	ms_MCoH,
	ms_MCo2HNa,
	ms_MCo2HK,
	ms_MCo3H,
	ms_MNiH,
	ms_MNi2HNa,
	ms_MNi2HK,
	ms_MNi3H,
	ms_MCuH,
	ms_MCu2HNa,
	ms_MCu2HK,
	ms_MCu3H,
	ms_MZnH,
	ms_MZn2HNa,
	ms_MZn2HK,
	ms_MZn3H,
	ms_MGa2H,
	ms_MGa3HNa,
	ms_MGa3HK,
	ms_MGa4H,
	fragmentIonTypeEnd
};


/**
	\brief Register vector<eFragmentIonType> by Qt.
*/
Q_DECLARE_METATYPE(vector<eFragmentIonType>);


/**
	\brief The structure for a detailed definition of a fragment ion type.
*/
struct fragmentDescription {

	/**
		\brief Name of the fragment.
	*/ 
	string name;


	/**
		\brief Summary formula of the fragment.
	*/ 
	string summary;


	/**
		\brief A map of atoms corresponding to the summary formula.
	*/
	map<string, int> summarymap;


	/**
		\brief Mass difference of the fragment.
	*/ 
	double massdifference;


	/**
		\brief True when the fragment is N-terminal.
	*/ 
	bool nterminal;


	/**
		\brief True when the fragment is C-terminal.
	*/ 
	bool cterminal;


	/**
		\brief True when the fragment is charged positively; false when the fragment is charged negatively.
	*/ 
	bool positive;


	/**
		\brief The multiplier (n) of M in [nM + H]+.
	*/ 
	int multiplier;


	/**
		\brief The default constructor.
	*/ 
	fragmentDescription() {
		clear();
	}


	/**
		\brief Clear the structure.
	*/ 
	void clear() {
		name = "";
		summary = "";
		summarymap.clear();
		massdifference = 0;
		nterminal = false;
		cterminal = false;
		positive = true;
		multiplier = 1;
	}


	/**
		\brief Store the structure into an output stream.
		\param os an output stream
	*/ 
	void store(ofstream& os);


	/**
		\brief Load the structure from an input stream.
		\param is an input stream
	*/ 
	void load(ifstream& is);

};


/**
	\brief Register vector<fragmentDescription> by Qt.
*/
Q_DECLARE_METATYPE(vector<fragmentDescription>);


/**
	\brief The detailed definitions of all supported fragment ion types.
*/
class cFragmentIons {

	map<eFragmentIonType,fragmentDescription> fragmentions;

public:


	/**
		\brief The constructor.
	*/ 
	cFragmentIons();


	/**
		\brief The constructor.
		\param cyclicnterminus true when the N-terminus of a linear peptide is cyclized
		\param cycliccterminus true when the C-terminus of a linear peptide is cyclized
		\param precursoradduct an adduct of a precursor ion
	*/ 
	cFragmentIons(bool cyclicnterminus, bool cycliccterminus, string& precursoradduct);


	/**
		\brief Overloaded operator [].
		\param iontype type of fragment ion
		\retval fragmentDescription reference to a structure with detailed information about the fragment ion
	*/ 
	fragmentDescription &operator[](eFragmentIonType iontype);


	/**
		\brief Recalculate all fragment ion types.
		\param cyclicnterminus true when the N-terminus of a linear peptide is cyclized
		\param cycliccterminus true when the C-terminus of a linear peptide is cyclized
		\param precursoradduct an adduct of a precursor ion
	*/ 
	void recalculateFragments(bool cyclicnterminus, bool cycliccterminus, string& precursoradduct);


	/**
		\brief Store the structure into an output stream.
		\param os an output stream
	*/ 
	void store(ofstream& os);


	/**
		\brief Load the structure from an input stream.
		\param is an input stream
	*/ 
	void load(ifstream& is);

};


/**
	\brief Register cFragmentIons by Qt.
*/
Q_DECLARE_METATYPE(cFragmentIons);                   


/**
	\brief Initialize fragment ion types for the de novo graph of cyclic peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfCyclicPeptides(vector<eFragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of linear peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfLinearPeptides(vector<eFragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of branched peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfTPeptides(vector<eFragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of branch-cyclic peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfBranchCyclicPeptides(vector<eFragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of a linear polyketide
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfLinearPolyketide(vector<eFragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of a cyclic polyketide
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfCyclicPolyketide(vector<eFragmentIonType>& fragmentions);


/**
	\brief The structure for a detailed definition of a neutral loss.
*/
struct neutralLoss {

	/**
		\brief Summary formula of the loss.
	*/
	string summary;


	/**
		\brief A map of atoms corresponding to the summary formula.
	*/
	map<string, int> summarymap;


	/**
		\brief Mass difference of the loss.
	*/
	double massdifference;


	/**
		\brief The default constructor.
	*/
	neutralLoss() {
		clear();
	}


	/**
		\brief Clear the structure.
	*/
	void clear() {
		summary = "";
		summarymap.clear();
		massdifference = 0;
	}


	/**
		\brief Store the structure into an output stream.
		\param os an output stream
	*/
	void store(ofstream& os);


	/**
		\brief Load the structure from an input stream.
		\param is an input stream
	*/
	void load(ifstream& is);

};


/**
	\brief The class representing a vector of default neutral losses.
*/
class cDefaultNeutralLosses {

	vector<string> neutrallosses;

public:

	/**
		\brief The constructor.
	*/
	cDefaultNeutralLosses();


	/**
		\brief Get the vector of neutral losses.
		\retval vector<string> vector of neutral losses
	*/	
	vector<string>& getNeutralLosses() {
		return neutrallosses;
	}

};


/**
	\brief Default neutral losses definitions.
*/
extern cDefaultNeutralLosses defaultneutrallosses;


/**
	\brief Load modifications from a plain text stream.
	\param stream reference to an input file stream
	\param modifications reference to a vector where modifications will be placed
	\param errormessage reference to a string where an error message might be stored
	\param ignoreerrors if true errors are ignored
	\retval int 0 when modifications were successfully loaded, -1 when an error occurred (\a errormessage is filled up)
*/ 
int loadModificationsFromPlainTextStream(ifstream &stream, vector<fragmentDescription>& modifications, string& errormessage, bool ignoreerrors);


/**
	\brief Store modifications into a plain text stream.
	\param stream reference to an output file stream
	\param modifications reference to a vector of modifications which will be stored
*/ 
void storeModificationsToPlainTextStream(ofstream &stream, vector<fragmentDescription>& modifications);


#endif

