#include "gui/cChromatogramWindowWidget.h"


double getGaussianArea(double sigma, double height) {
	return height * sigma * sqrt(2.0 * pi);
}


double getGaussianAreas(vector<double>& sigmas, vector<double>& heights) {
	if (sigmas.size() != heights.size()) {
		return 0;
	}

	double area = 0;
	size_t size = sigmas.size();
	for (size_t i = 0; i < size; i++) {
		area += getGaussianArea(sigmas[i], heights[i]);
	}

	return area;
}


double getExponentiallyModifiedGaussianArea(double x, double h, double ny, double sigma, double tau, bool fronting) {
	if (sigma == 0) {
		return 0;
	}

	if (tau == 0) {
		// to do - return Gaussian ?
		return 0;
	}

	//double y = computeExponentiallyModifiedGaussFunction(x, h, ny, sigma, tau, fronting);

	double t0, h0;
	t0 = ny + erfcxinv(tau / sigma * sqrt(2.0 / pi)) * sigma * sqrt(2.0) - sigma * sigma / tau;

	h0 = ny - t0;
	h0 *= h0;
	h0 = h * pow(econst, -h0 / (2.0 * sigma * sigma));

	h = h * h / h0;

	return getGaussianArea(sigma, h);
		
	//if (fronting) {
	//	ny = 2 * ny - t0;
	//}
	//else {
	//	ny = t0;
	//}

	//double area = y * 2 * tau;

	//double z;
	//if (fronting) {
	//	z = (-(ny - x) / sigma + sigma / tau) / sqrt(2.0);
	//}
	//else {
	//	z = ((ny - x) / sigma + sigma / tau) / sqrt(2.0);
	//}

	//if (z < 0) {
	//	if (fronting) {
	//		area /= pow(econst, -(ny - x) / tau + sigma * sigma / (2.0 * tau * tau));
	//	}
	//	else {
	//		area /= pow(econst, (ny - x) / tau + sigma * sigma / (2.0 * tau * tau));
	//	}
	//	area /= erfc(z);
	//}
	//else {
	//	area /= pow(econst, -(ny - x)*(ny - x) / (2.0 * sigma * sigma));
	//	area /= erfcx(z);
	//}
	//
	//return area;
}


double getExponentiallyModifiedGaussianAreas(vector<double>& xvalues, vector<double>& heights, vector<double>& nys, vector<double>& sigmas, vector<double>& taus, bool fronting) {
	if ((sigmas.size() != xvalues.size()) || (sigmas.size() != heights.size()) || (sigmas.size() != nys.size()) || (sigmas.size() != taus.size())) {
		return 0;
	}

	double area = 0;
	size_t size = sigmas.size();
	for (size_t i = 0; i < size; i++) {
		area += getExponentiallyModifiedGaussianArea(xvalues[i], heights[i], nys[i], sigmas[i], taus[i], fronting);
	}

	return area;
}


//double erfcx(double z) {
//	if (z > 26) {
//		return 1.0 / (z * sqrt(pi));
//	}
//	return pow(econst, z*z) * erfc(z);
//}


/*
	Based on: M. M. Shepherd and J. G. Laframboise, "Chebyshev Approximation of
	(1+2x)exp(x^2)erfc x in 0 <= x < INF", Mathematics of Computation, Vol. 36,
	No. 153, January 1981, pp. 249-253.
	https://stackoverflow.com/questions/39777360/accurate-computation-of-scaled-complementary-error-function-erfcx
*/
double erfcx(double x) {
	double a, d, e, m, p, q, r, s, t;

	a = fmax(x, 0.0 - x); // NaN preserving absolute value computation

	/* Compute q = (a-4)/(a+4) accurately. [0,INF) -> [-1,1] */
	m = a - 4.0;
	p = a + 4.0;
	r = 1.0 / p;
	q = m * r;
	t = fma(q + 1.0, -4.0, a);
	e = fma(q, -a, t);
	q = fma(r, e, q);

	/* Approximate (1+2*a)*exp(a*a)*erfc(a) as p(q)+1 for q in [-1,1] */
	p = 0x1.edcad78fc8044p-31;  //  8.9820305531190140e-10
	p = fma(p, q, 0x1.b1548f14735d1p-30); //  1.5764464777959401e-09
	p = fma(p, q, -0x1.a1ad2e6c4a7a8p-27); // -1.2155985739342269e-08
	p = fma(p, q, -0x1.1985b48f08574p-26); // -1.6386753783877791e-08
	p = fma(p, q, 0x1.c6a8093ac4f83p-24); //  1.0585794011876720e-07
	p = fma(p, q, 0x1.31c2b2b44b731p-24); //  7.1190423171700940e-08
	p = fma(p, q, -0x1.b87373facb29fp-21); // -8.2040389712752056e-07
	p = fma(p, q, 0x1.3fef1358803b7p-22); //  2.9796165315625938e-07
	p = fma(p, q, 0x1.7eec072bb0be3p-18); //  5.7059822144459833e-06
	p = fma(p, q, -0x1.78a680a741c4ap-17); // -1.1225056665965572e-05
	p = fma(p, q, -0x1.9951f39295cf4p-16); // -2.4397380523258482e-05
	p = fma(p, q, 0x1.3be1255ce180bp-13); //  1.5062307184282616e-04
	p = fma(p, q, -0x1.a1df71176b791p-13); // -1.9925728768782324e-04
	p = fma(p, q, -0x1.8d4aaa0099bc8p-11); // -7.5777369791018515e-04
	p = fma(p, q, 0x1.49c673066c831p-8);  //  5.0319701025945277e-03
	p = fma(p, q, -0x1.0962386ea02b7p-6);  // -1.6197733983519948e-02
	p = fma(p, q, 0x1.3079edf465cc3p-5);  //  3.7167515521269866e-02
	p = fma(p, q, -0x1.0fb06dfedc4ccp-4);  // -6.6330365820039094e-02
	p = fma(p, q, 0x1.7fee004e266dfp-4);  //  9.3732834999538536e-02
	p = fma(p, q, -0x1.9ddb23c3e14d2p-4);  // -1.0103906603588378e-01
	p = fma(p, q, 0x1.16ecefcfa4865p-4);  //  6.8097054254651804e-02
	p = fma(p, q, 0x1.f7f5df66fc349p-7);  //  1.5379652102610957e-02
	p = fma(p, q, -0x1.1df1ad154a27fp-3);  // -1.3962111684056208e-01
	p = fma(p, q, 0x1.dd2c8b74febf6p-3);  //  2.3299511862555250e-01

	/* Divide (1+p) by (1+2*a) ==> exp(a*a)*erfc(a) */
	d = a + 0.5;
	r = 1.0 / d;
	r = r * 0.5;
	q = fma(p, r, r); // q = (p+1)/(1+2*a)
	t = q + q;
	e = (p - q) + fma(t, -a, 1.0); // residual: (p+1)-q*(1+2*a)
	r = fma(e, r, q);

	/* Handle argument of infinity */
	if (a > 0x1.fffffffffffffp1023) r = 0.0;

	/* Handle negative arguments: erfcx(x) = 2*exp(x*x) - erfcx(|x|) */
	if (x < 0.0) {
		s = x * x;
		d = fma(x, x, -s);
		e = exp(s);
		r = e - r;
		r = fma(e, d + d, r);
		r = r + e;
		if (e > 0x1.fffffffffffffp1023) r = e; // avoid creating NaN
	}
	return r;
}


double erfcxinv(double z) {
	double value;

	if (z <= 1) {
		value = 1.0 / (z * sqrt(pi));
	}
	else {
		value = -sqrt(log(z));
	}

	for (int i = 0; i < 7; i++) {
		value = value - (erfcx(value) - z) / (2.0 * value * erfcx(value) - 2.0 / sqrt(pi));
	}

	return value;
}


double computeExponentiallyModifiedGaussFunction(double x, double h, double ny, double sigma, double tau, bool fronting) {
	if (sigma == 0) {
		return 0;
	}

	if (tau == 0) {
		// to do - return Gaussian ?
		return 0;
	}

	double t0, h0;
	t0 = ny + erfcxinv(tau / sigma * sqrt(2.0 / pi)) * sigma * sqrt(2.0) - sigma * sigma / tau;

	h0 = ny - t0;
	h0 *= h0;
	h0 = h * pow(econst, -h0 / (2.0 * sigma * sigma));

	if (fronting) {
		//x = x - ny + t0;
		ny = 2 * ny - t0;
	}
	else {
		//x = x + ny - t0;
		ny = t0;
	}

	h = h * h / h0;

	double z;
	if (fronting) {
		z = (-(ny - x) / sigma + sigma / tau) / sqrt(2.0);
	}
	else {
		z = ((ny - x) / sigma + sigma / tau) / sqrt(2.0);
	}
	
	double value;
	if (z < 0) {
		// Kalambet et al. equation (2)
		if (fronting) {
			value = -(ny - x) / tau + (sigma * sigma) / (2.0 * tau * tau);
		}
		else {
			value = (ny - x) / tau + (sigma * sigma) / (2.0 * tau * tau);
		}
		value = pow(econst, value);
		value = value * h * sigma * sqrt(pi / 2.0) / tau;
		value = value * erfc(z);
	}
	else {
		// Kalambet et al. equation (6)
		value = ny - x;
		value *= value;
		value = pow(econst, -value / (2.0 * sigma * sigma));
		value = value * h * sigma * sqrt(pi / 2.0) / tau;
		value = value * erfcx(z);
	}

	return value;
}


double computeExponentialFunction(double x, double base, double tau, bool fronting) {
	if (fronting) {
		return 1.0 / tau * pow(base, x / tau);
	}
	return 1.0 / tau * pow(base, -x / tau);
}


void calculateCaruana(vector<double>& x, vector<double>& y, double& ny, double& sigma, double& height) {
	long double a, b, c;
	long double n = (long double)x.size();
	
	long double sumx = 0;
	long double sumx2 = 0;
	long double sumx3 = 0;
	long double sumx4 = 0;
	
	long double sumlny = 0;
	long double sumxlny = 0;
	long double sumx2lny = 0;

	long double tmp;

	int size = (int)x.size();
	for (int i = 0; i < size; i++) {
		sumx += x[i];
		sumx2 += x[i] * x[i];
		sumx3 += x[i] * x[i] * x[i];
		sumx4 += x[i] * x[i] * x[i] * x[i];

		sumlny += log(y[i]);
		sumxlny += x[i] * log(y[i]);
		sumx2lny += x[i] * x[i] * log(y[i]);
	}

	ny = 0;
	sigma = 0;
	height = DBL_MAX;

	a = (sumlny * sumx4 - sumx2lny * sumx2) * (sumx2 * sumx4 - sumx3 * sumx3) - (sumxlny * sumx4 - sumx2lny * sumx3) * (sumx * sumx4 - sumx3 * sumx2);
	tmp = (n * sumx4 - sumx2 * sumx2) * (sumx2 * sumx4 - sumx3 * sumx3) - (sumx * sumx4 - sumx2 * sumx3) * (sumx * sumx4 - sumx3 * sumx2);
	if (tmp == 0) {
		return;
	}
	a /= tmp;

	b = sumxlny * sumx4 - sumx2lny * sumx3 - a * (sumx * sumx4 - sumx2 * sumx3);
	tmp = sumx2 * sumx4 - sumx3 * sumx3;
	if (tmp == 0) {
		return;
	}
	b /= tmp;

	c = sumx2lny - a * sumx2 - b * sumx3;
	tmp = sumx4;
	if (tmp == 0) {
		return;
	}
	c /= tmp;

	//cout << "a = " << a << "; b = " << b << "; c = " << c << endl;

	//cout << n * a + sumx * b + sumx2 * c << " = " << sumlny << endl;
	//cout << sumx * a + sumx2 * b + sumx3 * c << " = " << sumxlny << endl;
	//cout << sumx2 * a + sumx3 * b + sumx4 * c << " = " << sumx2lny << endl;
	//cout << endl;

	if (c >= 0) {
		return;
	}

	ny = -b / (2.0 * c);

	sigma = -1.0 / (2.0 * c);
	sigma = sqrt(sigma);

	height = a - b * b / (4.0 * c);
	height = pow(econst, height);
}


void calculateGuo(vector<double>& x, vector<double>& y, int iterations, double& ny, double& sigma, double& height) {
	long double a, b, c;

	long double sumy2;

	long double sumxy2;
	long double sumx2y2;
	long double sumx3y2;
	long double sumx4y2;

	long double sumy2lny;
	long double sumxy2lny;
	long double sumx2y2lny;

	long double y2;
	long double tmp;

	long double expr1;
	long double expr2;
	long double expr3;

	int xsize;

	ny = 0;
	sigma = 0;
	height = DBL_MAX;

	double S;
	double oldS = DBL_MAX;
	double oldny, oldsigma, oldheight;

	for (int i = 0; i < iterations; i++) {

		sumy2 = 0;

		sumxy2 = 0;
		sumx2y2 = 0;
		sumx3y2 = 0;
		sumx4y2 = 0;

		sumy2lny = 0;
		sumxy2lny = 0;
		sumx2y2lny = 0;

		y2 = 0;

		xsize = (int)x.size();
		for (int j = 0; j < xsize; j++) {
			if (y[j] <= 1) {
				return;
			}

			if (i == 0) {
				y2 = y[j] * y[j];
			}
			else {
				y2 = a + b * x[j] + c * x[j] * x[j];
				y2 = pow(econst, y2);
				y2 *= y2;
			}

			sumy2 += y2;

			sumxy2 += x[j] * y2;
			sumx2y2 += x[j] * x[j] * y2;
			sumx3y2 += x[j] * x[j] * x[j] * y2;
			sumx4y2 += x[j] * x[j] * x[j] * x[j] * y2;

			sumy2lny += y2 * log(y[j]);
			sumxy2lny += x[j] * y2 * log(y[j]);
			sumx2y2lny += x[j] * x[j] * y2 * log(y[j]);
		}

		expr1 = sumx2y2 * sumx4y2 - sumx3y2 * sumx3y2;
		expr2 = sumxy2lny * sumx4y2 - sumx2y2lny * sumx3y2;
		expr3 = sumxy2 * sumx4y2 - sumx3y2 * sumx2y2;

		a = (sumy2lny * sumx4y2 - sumx2y2lny * sumx2y2) * expr1 - expr2 * expr3;
		tmp = (sumy2 * sumx4y2 - sumx2y2 * sumx2y2) * expr1 - expr3 * expr3;
		if (tmp == 0) {
			return;
		}
		a /= tmp;

		b = expr2 - a * expr3;
		tmp = expr1;
		if (tmp == 0) {
			return;
		}
		b /= tmp;

		c = sumx2y2lny - a * sumx2y2 - b * sumx3y2;
		tmp = sumx4y2;
		if (tmp == 0) {
			return;
		}
		c /= tmp;

		//cout << "iteration: " << i + 1 << endl;
		//cout << "a = " << a << "; b = " << b << "; c = " << c << endl;
		//cout << sumy2 * a + sumx * b + sumx2 * c << " = " << sumlny << endl;
		//cout << sumx * a + sumx2 * b + sumx3 * c << " = " << sumxlny << endl;
		//cout << sumx2 * a + sumx3 * b + sumx4 * c << " = " << sumx2lny << endl;

		if (c >= 0) {
			return;
		}

		ny = -b / (2.0 * c);

		sigma = -1.0 / (2.0 * c);
		sigma = sqrt(sigma);

		height = a - b * b / (4.0 * c);
		height = pow(econst, height);

		//cout << "iteration: " << i + 1 << ", ";
		//cout << "ny: " << ny << ", ";
		//cout << "sigma: " << sigma << ", ";
		//cout << "height: " << height << ", ";
		//cout << endl;

		if (xsize > 0) {
			if ((ny < x[0]) || (ny > x[xsize - 1])) {
				ny = 0;
				sigma = 0;
				height = DBL_MAX;
				return;
			}
		}

		S = 0;
		for (int j = 0; j < xsize; j++) {
			tmp = y[j] - computeGaussFunction(x[j], height, ny, sigma);
			S += tmp * tmp;
		}

		//cout << "iteration: " << i + 1 << ", ";
		//cout << "S: " << S << ", oldS: " << oldS << endl;

		if (i > 0) {
			if (oldS <= S) {
				ny = oldny;
				sigma = oldsigma;
				height = oldheight;
				return;
			}
		}

		oldny = ny;
		oldsigma = sigma;
		oldheight = height;
		oldS = S;

	}

	//cout << endl;
}


void calculateBaseTau(vector<double>& x, vector<double>& y, int iterations, bool fronting, double ynorm, double& base, double& tau) {
	base = 0;
	tau = 0;

	if (x.size() != y.size()) {
		return;
	}

	long double sumx;
	long double sumx2;

	long double sumlny;
	long double sumxlny;

	long double y2;
	long double sumy2;

	long double A, B;
	long double tmp;

	double S;
	double oldS = DBL_MAX;
	double oldbase, oldtau;
	double x0;

	size_t xsize;

	if (fronting) {
		
		size_t k = 0;
		while (k + 1 < x.size()) {
			if (y[k + 1] < y[k]) {
				x.erase(x.begin() + k + 1, x.end());
				y.erase(y.begin() + k + 1, y.end());
				break;
			}
			k++;
		}

		xsize = (int)x.size();

		if (xsize == 0) {
			return;
		}
		
		double maxy = 0;
		for (int i = 0; i < xsize; i++) {
			if (y[i] > maxy) {
				maxy = y[i];
			}
		}

		if (maxy == 0) {
			return;
		}

		x0 = x[0];
		for (int i = 0; i < xsize; i++) {
			x[i] -= x0;
			y[i] = y[i] / maxy;
		}

		for (int i = 0; i < iterations; i++) {
			y2 = 0;
			sumy2 = 0;

			sumx = 0;
			sumx2 = 0;

			sumlny = 0;
			sumxlny = 0;

			for (int j = 0; j < xsize; j++) {
				if (i == 0) {
					y2 = y[j] * y[j];
				}
				else {
					y2 = A + B * x[j];
					y2 = pow(econst, y2);
					y2 *= y2;
				}

				sumy2 += y2;

				sumx += x[j] * y2;
				sumx2 += x[j] * x[j] * y2;

				sumlny += log(y[j]) * y2;
				sumxlny += x[j] * log(y[j]) * y2;
			}

			tmp = sumx2 * sumy2 - sumx * sumx;
			if (tmp == 0) {
				return;
			}
			B = (sumxlny * sumy2 - sumlny * sumx) / tmp;

			if (sumy2 == 0) {
				return;
			}
			A = (sumlny - B * sumx) / sumy2;

			tau = ynorm / pow(econst, A);
			base = pow(econst, B * tau);

			if (tau <= 0) {
				tau = 0;
				base = 0;
				return;
			}

			S = 0;
			for (int j = 0; j < xsize; j++) {
				tmp = y[j] - computeExponentialFunction(x[j], base, tau, fronting);
				S += tmp * tmp;
			}

			cout << "fronting, iteration: " << i + 1 << ", base: " << base << ", tau: " << tau << ", S: " << S << endl;

			if (i > 0) {
				if (oldS <= S) {
					base = oldbase;
					tau = oldtau;
					return;
				}
			}

			oldbase = base;
			oldtau = tau;
			oldS = S;

		}

	}
	else {
		
		while ((y.size() > 1) && (y[0] < y[1])) {
			x.erase(x.begin());
			y.erase(y.begin());
		}

		xsize = (int)x.size();

		double maxy = 0;
		for (int i = 0; i < xsize; i++) {
			if (y[i] > maxy) {
				maxy = y[i];
			}
		}

		if (maxy == 0) {
			return;
		}

		x0 = x[0];
		for (int i = 0; i < xsize; i++) {
			x[i] -= x0;
			y[i] = y[i] / maxy;
		}

		for (int i = 0; i < iterations; i++) {
			y2 = 0;
			sumy2 = 0;

			sumx = 0;
			sumx2 = 0;

			sumlny = 0;
			sumxlny = 0;

			for (int j = 0; j < xsize; j++) {
				if (i == 0) {
					y2 = y[j] * y[j];
				}
				else {
					y2 = A + B * x[j];
					y2 = pow(econst, y2);
					y2 *= y2;
				}

				sumy2 += y2;

				sumx += x[j] * y2;
				sumx2 += x[j] * x[j] * y2;

				sumlny += log(y[j]) * y2;
				sumxlny += x[j] * log(y[j]) * y2;
			}
		
			tmp = sumx2 * sumy2 - sumx * sumx;
			if (tmp == 0) {
				return;
			}
			B = (sumxlny * sumy2 - sumlny * sumx) / tmp;

			if (sumy2 == 0) {
				return;
			}
			A = (sumlny - B * sumx) / sumy2;

			tau = ynorm / pow(econst, A);
			base = pow(econst, -B * tau);

			if (tau <= 0) {
				tau = 0;
				base = 0;
				return;
			}

			S = 0;
			for (int j = 0; j < xsize; j++) {
				tmp = y[j] - computeExponentialFunction(x[j], base, tau, fronting);
				S += tmp * tmp;
			}

			cout << "tailing, iteration: " << i + 1 << ", base: " << base << ", tau: " << tau << ", S: " << S << endl;

			if (i > 0) {
				if (oldS <= S) {
					base = oldbase;
					tau = oldtau;
					return;
				}
			}

			oldbase = base;
			oldtau = tau;
			oldS = S;

		}

	}

}


void calculateTauUsingWeightedAverage(vector<double>& x, vector<double>& y, bool fronting, double& tau) {
	tau = 1;

	if (x.size() != y.size()) {
		return;
	}

	double x0;
	size_t xsize;

	double maxy = 0;
	double avg = 0;
	double div = 0;

	if (fronting) {
		size_t k = 0;
		while (k + 1 < x.size()) {
			if (y[k + 1] < y[k]) {
				x.erase(x.begin() + k + 1, x.end());
				y.erase(y.begin() + k + 1, y.end());
				break;
			}
			k++;
		}
	}
	else {
		while ((y.size() > 1) && (y[0] < y[1])) {
			x.erase(x.begin());
			y.erase(y.begin());
		}
	}

	xsize = (int)x.size();

	for (int i = 0; i < xsize; i++) {
		if (y[i] > maxy) {
			maxy = y[i];
		}
	}

	if (maxy == 0) {
		return;
	}

	x0 = x[0];
	for (int i = 0; i < xsize; i++) {
		x[i] -= x0;
		y[i] = y[i] / maxy;

		avg += x[i] * y[i];
		div += y[i];
	}

	avg /= div;
	tau = avg;
}


void calculateGaussianParameters(cPeaksList& eicchromatogram, vector<double>& rtimes, int timeunit, bool absoluteintensity, bool usert, vector<double>& nys, vector<double>& sigmas, vector<double>& as) {
	if (eicchromatogram.size() != rtimes.size()) {
		return;
	}

	if ((rtimes.size() > 0) && (rtimes[rtimes.size() - 1] == 0)) {
		usert = false;
	}

	double ny, sigma, height;

	vector<double> x;
	vector<double> y;

	nys.clear();
	sigmas.clear();
	as.clear();

	int eicsize = eicchromatogram.size();
	for (int i = 0; i < eicsize; i++) {
		if (absoluteintensity) {
			if (eicchromatogram[i].absoluteintensity > 0) {
				if (usert) {
					if (timeunit == 1) {
						x.push_back(rtimes[i] * 60.0);
					}
					else {
						x.push_back(rtimes[i]);
					}
					y.push_back(eicchromatogram[i].absoluteintensity);
				}
				else {
					x.push_back(eicchromatogram[i].mzratio);
					y.push_back(eicchromatogram[i].absoluteintensity);
				}
			}

			if ((eicchromatogram[i].absoluteintensity == 0) && (x.size() > 0)) {
				if ((i + 4 < eicsize) && (eicchromatogram[i + 1].absoluteintensity == 0) && (eicchromatogram[i + 2].absoluteintensity == 0) && (eicchromatogram[i + 3].absoluteintensity == 0) && (eicchromatogram[i + 4].absoluteintensity == 0)) {
					//calculateCaruana(x, y, ny, sigma, height);
					calculateGuo(x, y, 10, ny, sigma, height);
					nys.push_back(ny);
					sigmas.push_back(sigma);
					as.push_back(height);
					x.clear();
					y.clear();
				}
			}
		}
		else {
			if (eicchromatogram[i].relativeintensity > 0) {
				if (usert) {
					if (timeunit == 1) {
						x.push_back(rtimes[i] * 60.0);
					}
					else {
						x.push_back(rtimes[i]);
					}
					y.push_back(eicchromatogram[i].relativeintensity);
				}
				else {
					x.push_back(eicchromatogram[i].mzratio);
					y.push_back(eicchromatogram[i].relativeintensity);
				}
			}

			if ((eicchromatogram[i].relativeintensity == 0) && (x.size() > 0)) {
				if ((i + 4 < eicsize) && (eicchromatogram[i + 1].relativeintensity == 0) && (eicchromatogram[i + 2].relativeintensity == 0) && (eicchromatogram[i + 3].relativeintensity == 0) && (eicchromatogram[i + 4].relativeintensity == 0)) {
					//calculateCaruana(x, y, ny, sigma, height);
					calculateGuo(x, y, 10, ny, sigma, height);
					nys.push_back(ny);
					sigmas.push_back(sigma);
					as.push_back(height);
					x.clear();
					y.clear();
				}
			}
		}
	}

	if (x.size() > 0) {
		//calculateCaruana(x, y, ny, sigma, height);
		calculateGuo(x, y, 10, ny, sigma, height);
		nys.push_back(ny);
		sigmas.push_back(sigma);
		as.push_back(height);
	}
}


void calculateExponentialParameters(cPeaksList& eicchromatogram, vector<double>& rtimes, int timeunit, bool absoluteintensity, bool usert, bool fronting, double ynorm, vector<double>& bases, vector<double>& taus) {
	if (eicchromatogram.size() != rtimes.size()) {
		return;
	}

	if ((rtimes.size() > 0) && (rtimes[rtimes.size() - 1] == 0)) {
		usert = false;
	}

	double base = 0;
	double tau = 0;

	vector<double> x;
	vector<double> y;

	bases.clear();
	taus.clear();

	int eicsize = eicchromatogram.size();
	for (int i = 0; i < eicsize; i++) {
		if (absoluteintensity) {
			if (eicchromatogram[i].absoluteintensity > 0) {
				if (usert) {
					if (timeunit == 1) {
						x.push_back(rtimes[i] * 60.0);
					}
					else {
						x.push_back(rtimes[i]);
					}
					y.push_back(eicchromatogram[i].absoluteintensity);
				}
				else {
					x.push_back(eicchromatogram[i].mzratio);
					y.push_back(eicchromatogram[i].absoluteintensity);
				}
			}

			if ((eicchromatogram[i].absoluteintensity == 0) && (x.size() > 0)) {
				if ((i + 4 < eicsize) && (eicchromatogram[i + 1].absoluteintensity == 0) && (eicchromatogram[i + 2].absoluteintensity == 0) && (eicchromatogram[i + 3].absoluteintensity == 0) && (eicchromatogram[i + 4].absoluteintensity == 0)) {
					calculateTauUsingWeightedAverage(x, y, fronting, tau);
					//calculateBaseTau(x, y, 10, fronting, ynorm, base, tau);
					bases.push_back(base);
					taus.push_back(tau);
					x.clear();
					y.clear();
				}
			}
		}
		else {
			if (eicchromatogram[i].relativeintensity > 0) {
				if (usert) {
					if (timeunit == 1) {
						x.push_back(rtimes[i] * 60.0);
					}
					else {
						x.push_back(rtimes[i]);
					}
					y.push_back(eicchromatogram[i].relativeintensity);
				}
				else {
					x.push_back(eicchromatogram[i].mzratio);
					y.push_back(eicchromatogram[i].relativeintensity);
				}
			}

			if ((eicchromatogram[i].relativeintensity == 0) && (x.size() > 0)) {
				if ((i + 4 < eicsize) && (eicchromatogram[i + 1].relativeintensity == 0) && (eicchromatogram[i + 2].relativeintensity == 0) && (eicchromatogram[i + 3].relativeintensity == 0) && (eicchromatogram[i + 4].relativeintensity == 0)) {
					calculateTauUsingWeightedAverage(x, y, fronting, tau); 
					//calculateBaseTau(x, y, 10, fronting, ynorm, base, tau);
					bases.push_back(base);
					taus.push_back(tau);
					x.clear();
					y.clear();
				}
			}
		}
	}

	if (x.size() > 0) {
		calculateTauUsingWeightedAverage(x, y, fronting, tau); 
		//calculateBaseTau(x, y, 10, fronting, ynorm, base, tau);
		bases.push_back(base);
		taus.push_back(tau);
	}
}


cChromatogramWindowWidget::cChromatogramWindowWidget(cTheoreticalSpectrumList& listoftheoreticalspectra, QWidget* parent) {
	this->listoftheoreticalspectra = &listoftheoreticalspectra;
	this->parent = parent;

	ticchromatogram.clear();
	eicchromatogram.clear();
	rtimes.clear();

	currentscale = 1;
	factor = 0.2;

	pressedx = -1;
	pressedy = -1;
	currentx = 0;
	currenty = 0;
	mintime = 0;
	maxtime = 0;
	minscan = 0;
	maxscan = 0;

	topmargin = 0;
	bottommargin = 0;
	leftmargin = 0;
	rightmargin = 0;

	firstshow = true;
	enablemouseselection = true;

	retentiontime = true;
	absoluteintensity = true;
	peakshape = 0;
	rawdatastate = false;
	hidetic = true;
	hideeic = false;
	hidelabels = false;

	scene = new QGraphicsScene(this);

	zoomgroup = new QGraphicsItemGroup();
	zoomrect = new QGraphicsRectItem();
	zoomgroup->addToGroup(zoomrect);
	zoomsimpletextitem = new QGraphicsSimpleTextItem();
	zoomgroup->addToGroup(zoomsimpletextitem);
	scene->addItem(zoomgroup);

	setScene(scene);
	
	origwidth = viewport()->width() * 4 / 5;
	origheight = viewport()->height() * 3 / 5;

	// a visualization fix
	origwidth = (origwidth / 10) * 10;
	origheight = (origheight / 10) * 10;

	originalmatrix = matrix();
}


cChromatogramWindowWidget::~cChromatogramWindowWidget() {
	delete scene;
}


void cChromatogramWindowWidget::exportToPDF(QString filename, bool postscript) {
	QPrinter printer;
	if (postscript) {
		printer.setPaperSize(QSizeF(scene->width() + 100, scene->height() + 100), QPrinter::DevicePixel);
		printer.setPageMargins(50, 50, 50, 50, QPrinter::DevicePixel);
	}
	else {
		printer.setPaperSize(QSizeF(scene->width(), scene->height()), QPrinter::DevicePixel);
		printer.setPageMargins(0, 0, 0, 0, QPrinter::DevicePixel);
	}
	printer.setOutputFormat(QPrinter::NativeFormat);
	printer.setOutputFileName(filename);

	QPainter painter;
	if (!painter.begin(&printer)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		#if OS_TYPE == UNX
			errstr += "\nDo you have a properly configured print server (sudo apt-get install cups-pdf) ?";
		#endif
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
	scene->render(&painter);
	painter.end();
}


void cChromatogramWindowWidget::exportToSVG(QString filename) {
	QSvgGenerator generator;
	generator.setFileName(filename);
	generator.setSize(QSize(scene->width(), scene->height()));
	generator.setViewBox(QRect(0, 0, scene->width(), scene->height()));

	QPainter painter;
	if (!painter.begin(&generator)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	painter.fillRect(QRect(0, 0, scene->width(), scene->height()), Qt::white);
	scene->render(&painter);
	painter.end();
}


void cChromatogramWindowWidget::exportToPNG(QString filename) {
	QImage image(scene->width(), scene->height(), QImage::Format_ARGB32);
	image.fill(Qt::white);

	QPainter painter;
	if (!painter.begin(&image)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	scene->render(&painter);
	painter.end();
	image.save(filename);
}


void cChromatogramWindowWidget::recalculateTICChromatogram(int activefileid) {
	int spectrumcount, peakscount;
	unsigned long long intensity;
	cPeak chromatogrampeak;

	ticchromatogram.clear();
	spectrumcount = listoftheoreticalspectra->size(activefileid);

	rtimes.clear();
	
	QProgressDialog progress("Updating...", /*"Cancel"*/0, 0, spectrumcount, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::ApplicationModal);
	progress.setValue(0);

	rtimes.resize(spectrumcount);
	
	for (int i = 0; i < spectrumcount; i++) {
		intensity = 0;
		peakscount = listoftheoreticalspectra->get(activefileid, i).getExperimentalSpectrum().size();
		for (int j = 0; j < peakscount; j++) {
			intensity += listoftheoreticalspectra->get(activefileid, i).getExperimentalSpectrum()[j].absoluteintensity;
		}
		chromatogrampeak.clear();
		chromatogrampeak.mzratio = (double)(ticchromatogram.size() + 1);
		chromatogrampeak.absoluteintensity = intensity;
		ticchromatogram.add(chromatogrampeak);

		rtimes[i] = listoftheoreticalspectra->get(activefileid, i).getExperimentalSpectrum().getRetentionTime();

		progress.setValue(i);
	}

	if ((spectrumcount > 0) && (rtimes[rtimes.size() - 1] == 0)) {
		for (int i = 0; i < spectrumcount; i++) {
			rtimes[i] = i + 1;
		}
	}

	ticchromatogram.normalizeIntenzity();

	progress.setValue(spectrumcount);

	resetScanIDInterval();
}


void cChromatogramWindowWidget::setEICChromatogram(cPeaksList eic) {
	eicchromatogram = eic;

	bool autoupdate = true;
	if (autoupdate) {
		int minscan = 0;
		int maxscan = eicchromatogram.size();

		for (int i = 0; i < eicchromatogram.size(); i++) {
			if (eicchromatogram[i].absoluteintensity > 0) {
				minscan = i + 1;
				break;
			}
		}

		for (int i = eicchromatogram.size() - 1; i >= 0; i--) {
			if (eicchromatogram[i].absoluteintensity > 0) {
				maxscan = i + 1;
				break;
			}
		}

		setScanIDInterval(minscan - 10, maxscan + 10);
	}
	else {
		redrawScene();
	}
}


void cChromatogramWindowWidget::wheelEvent(QWheelEvent *event) {
	int part, newmin, newmax;
	if (event->delta() > 0) {
		part = max(abs(maxscan - minscan) / 10, 1);
		newmin = minscan + part;
		newmax = maxscan - part;
	}
	else {
		part = max(abs(maxscan - minscan) / 8, 1);
		newmin = max(minscan - part, 1);
		newmax = min(maxscan + part, ticchromatogram.size());
	}

	if (newmin < newmax) {
		minscan = newmin;
		maxscan = newmax;

		emit updateScanIDInterval(minscan, maxscan);

		if ((rtimes.size() > 0) && (rtimes.size() == ticchromatogram.size())) {
			mintime = rtimes[minscan - 1];
			maxtime = rtimes[maxscan - 1];
			emit updateRetentionTimeInterval(mintime, maxtime);
		}


		redrawScene();
	}

	event->accept();
}


void cChromatogramWindowWidget::mouseMoveEvent(QMouseEvent *event) {
	QGraphicsView::mouseMoveEvent(event);

	if ((pressedx != -1) && (pressedy != -1)) {
		QPointF p = mapToScene(event->x(), event->y());
		currentx = (int)p.x();
		currenty = (int)p.y();

		if (enablemouseselection) {
			updateZoomGroup();
		}
		else {
			calculateMinMaxScan();

			emit updateScanIDInterval(minscan, maxscan);

			if ((rtimes.size() > 0) && (rtimes.size() == ticchromatogram.size())) {
				mintime = rtimes[minscan - 1];
				maxtime = rtimes[maxscan - 1];
				emit updateRetentionTimeInterval(mintime, maxtime);
			}

			pressedx = currentx;
			pressedy = currenty;

			redrawScene();
		}
	}

	event->accept();
}


void cChromatogramWindowWidget::mouseReleaseEvent(QMouseEvent *event) {
	QGraphicsView::mouseReleaseEvent(event);

	if (pressedx == currentx) {
		pressedx = -1;
		currentx = -1;

		redrawScene();
	}

	if ((event->button() == Qt::LeftButton) && (pressedx != -1) && (pressedy != -1)) {
		calculateMinMaxScan();

		emit updateScanIDInterval(minscan, maxscan);

		if ((rtimes.size() > 0) && (rtimes.size() == ticchromatogram.size())) {
			mintime = rtimes[minscan - 1];
			maxtime = rtimes[maxscan - 1];
			emit updateRetentionTimeInterval(mintime, maxtime);
		}

		pressedx = -1;
		pressedy = -1;

		redrawScene();
	}

	event->accept();
}


void cChromatogramWindowWidget::mousePressEvent(QMouseEvent *event) {
	QGraphicsView::mousePressEvent(event);

	if (event->button() == Qt::LeftButton) {
		QPointF p = mapToScene(event->x(), event->y());
		pressedx = (int)p.x();
		pressedy = (int)p.y();

		currentx = pressedx;
		currenty = pressedy;

		if (enablemouseselection) {
			updateZoomGroup();
		}
	}

	if (event->button() == Qt::RightButton) {
		pressedx = -1;
		pressedy = -1;

		redrawScene();
	}

	if (enablemouseselection) {
		if (event->button() == Qt::MiddleButton) {
			pressedx = -1;
			pressedy = -1;

			resetScanIDInterval();

			redrawScene();
		}
	}

	event->accept();
}


void cChromatogramWindowWidget::mouseDoubleClickEvent(QMouseEvent *event) {
	QGraphicsView::mouseDoubleClickEvent(event);
	int currentscan;

	if (event->button() == Qt::LeftButton) {
		QPointF p = mapToScene(event->x(), event->y());
		pressedx = (int)p.x();
		pressedy = (int)p.y();

		currentx = pressedx;
		currenty = pressedy;

		currentscan = getScanIDFromXPosition(currentx + origwidth / max(1, 2 * (maxscan - minscan + 1)), origwidth);

		emit chromatogramWidgetDoubleClicked(currentscan - 1);

		pressedx = -1;
		currentx = -1;

		redrawScene();
	}

	event->accept();
}


void cChromatogramWindowWidget::showEvent(QShowEvent *event) {
	QGraphicsView::showEvent(event);

	if (firstshow) {
		normalSize();
		firstshow = false;
	}
}


void cChromatogramWindowWidget::resizeEvent(QResizeEvent *event) {
	QGraphicsView::resizeEvent(event);

	origwidth = viewport()->width() * 4 / 5;
	origheight = viewport()->height() * 3 / 5;

	// a visualization fix
	origwidth = (origwidth / 10) * 10;
	origheight = (origheight / 10) * 10;

	redrawScene();
}


int cChromatogramWindowWidget::getScanIDFromXPosition(int x, int w) {
	int scanid = (double)(x - leftmargin) / (double)(w - leftmargin - rightmargin)*(maxscan - minscan + 1) + minscan - 1;
	return min(max(1, scanid), (int)ticchromatogram.size());
}


int cChromatogramWindowWidget::getXPositionFromScanID(int scanid, int w) {
	double val = scanid - minscan + 1;
	val /= maxscan - minscan + 1;
	val *= double(w - leftmargin - rightmargin);
	return (int)val + leftmargin;
}


void cChromatogramWindowWidget::redrawScene() {

	QGraphicsSimpleTextItem* simpletext;
	QGraphicsTextItem* text;
	QGraphicsLineItem* line;
	qreal tx, ty;
	int x;
	double y;

	int xstep, ystep;

	int w = origwidth;
	int h = origheight;
	int rulergranularity = 10;

	QFont myFont("Arial", 8);
	QFontMetrics fm(myFont);

	double maxintensitytic = 0;
	double maxintensityeic = 0;

	int peakscount = 0;

	int printscan;
	bool printintensity;

	vector<double> nys;
	vector<double> sigmas;
	vector<double> as;
	vector<double> bases;
	vector<double> taus;
	cPeaksList localeic = eicchromatogram;

	bool fronting = false;
	if (peakshape == 2) {
		fronting = true;
	}

	if (!hidetic) {
		if (absoluteintensity) {
			maxintensitytic = ticchromatogram.getMaximumAbsoluteIntensityFromMZInterval((double)minscan, (double)maxscan, false, false, other, false);
		}
		else {
			maxintensitytic = ticchromatogram.getMaximumRelativeIntensityFromMZInterval((double)minscan, (double)maxscan, false, false, other, false);
		}
	}

	if (!hideeic) {
		if (absoluteintensity) {
			maxintensityeic = localeic.getMaximumAbsoluteIntensityFromMZInterval((double)minscan, (double)maxscan, false, false, other, false);
		}
		else {
			maxintensityeic = localeic.getMaximumRelativeIntensityFromMZInterval((double)minscan, (double)maxscan, false, false, other, false);
		}
	}

	double maxintensity = max(maxintensitytic, maxintensityeic);

	if (rawdatastate && !hideeic && (maxscan > minscan - 1)) {
		size_t eicsize = localeic.size();
		for (int i = 0; i < eicsize; i++) {
			if ((localeic[i].mzratio < (double)minscan) || (localeic[i].mzratio > (double)maxscan)) {
				localeic[i].absoluteintensity = 0;
				localeic[i].relativeintensity = 0;
			}
		}

		calculateGaussianParameters(localeic, rtimes, 1, absoluteintensity, false, nys, sigmas, as);

		double newmaxintensity = maxintensity;
		for (int i = 0; i < (int)nys.size(); i++) {
			if (as[i] / maxintensity > 2 * maxintensity) {
				nys[i] = 0;
				sigmas[i] = 0;
				as[i] = 0;
			}

			if (as[i] > newmaxintensity) {
				newmaxintensity = as[i];
			}
		}

		if (absoluteintensity) {
			maxintensity = max(maxintensity, newmaxintensity);
		}
		else {
			if ((newmaxintensity > maxintensity) && (newmaxintensity > 0)) {
				for (int i = 0; i < localeic.size(); i++) {
					localeic[i].relativeintensity = localeic[i].relativeintensity * maxintensity / newmaxintensity;
				}

				for (int i = 0; i < (int)as.size(); i++) {
					as[i] = as[i] * maxintensity / newmaxintensity;
				}
			}
		}
	}

	scene->removeItem(zoomgroup);
	scene->clear();

	if ((ticchromatogram.size() < 2) || (hidetic && (localeic.size() < 2))) {
		simpletext = scene->addSimpleText("The chromatogram is not available.", myFont);
		simpletext->setPos(0, 0);
		scene->setSceneRect(scene->itemsBoundingRect());

		zoomgroup->setVisible(false);
		scene->addItem(zoomgroup);

		return;
	}

	zoomgroup->setVisible(false);
	scene->addItem(zoomgroup);

	// x axis
	scene->addLine(leftmargin, h - bottommargin, w - rightmargin, h - bottommargin, QPen(Qt::black, 2, Qt::SolidLine));

	// x axis ruler
	xstep = (w - leftmargin - rightmargin) / rulergranularity;
	for (int i = 0; i < rulergranularity; i++) {
		scene->addLine(leftmargin + xstep * i, h - bottommargin, leftmargin + xstep * i, h - bottommargin + 10, QPen(Qt::black, 2, Qt::SolidLine));
	}
	scene->addLine(w - rightmargin, h - bottommargin, w - rightmargin, h - bottommargin + 10, QPen(Qt::black, 2, Qt::SolidLine));

	printscan = minscan - 1;
	if (retentiontime) {
		if (printscan - 1 >= 0) {
			simpletext = scene->addSimpleText(QString::number(rtimes[printscan - 1]), myFont);
		}
		else {
			simpletext = scene->addSimpleText(QString::number(0), myFont);
		}
	}
	else {
		simpletext = scene->addSimpleText(QString::number(printscan), myFont);
	}
	simpletext->setPos(QPointF(leftmargin - simpletext->boundingRect().width() / 2, h - bottommargin + 12));

	if (maxscan - minscan > rulergranularity) {
		xstep = (w - leftmargin - rightmargin) / rulergranularity;
		for (int i = 1; i < rulergranularity; i++) {
			printscan = (double)minscan + (double)(maxscan - minscan) / (double)rulergranularity * (double)i;
			if (retentiontime) {
				if (printscan - 1 >= 0) {
					simpletext = scene->addSimpleText(QString::number(rtimes[printscan - 1]), myFont);
				}
				else {
					simpletext = scene->addSimpleText(QString::number(0), myFont);
				}
			}
			else {
				simpletext = scene->addSimpleText(QString::number(printscan), myFont);
			}
			simpletext->setPos(QPointF(leftmargin + xstep * i - simpletext->boundingRect().width() / 2, h - bottommargin + 12));
		}
	}

	printscan = maxscan;
	if (retentiontime) {
		if (printscan - 1 >= 0) {
			simpletext = scene->addSimpleText(QString::number(rtimes[printscan - 1]), myFont);
		}
		else {
			simpletext = scene->addSimpleText(QString::number(0), myFont);
		}
	}
	else {
		simpletext = scene->addSimpleText(QString::number(printscan), myFont);
	}
	simpletext->setPos(QPointF(w - rightmargin - simpletext->boundingRect().width() / 2, h - bottommargin + 12));

	// y axis
	scene->addLine(leftmargin, h - bottommargin, leftmargin, h - bottommargin - std::max(h - topmargin - bottommargin, 0), QPen(Qt::black, 2, Qt::SolidLine));

	// y axis ruler
	ystep = (h - topmargin - bottommargin) / rulergranularity;
	for (int i = 0; i < rulergranularity; i++) {
		scene->addLine(leftmargin - 10, h - bottommargin - ystep * i, leftmargin, h - bottommargin - ystep * i, QPen(Qt::black, 2, Qt::SolidLine));
	}
	scene->addLine(leftmargin - 10, h - bottommargin - std::max(h - topmargin - bottommargin, 0), leftmargin, h - bottommargin - std::max(h - topmargin - bottommargin, 0), QPen(Qt::black, 2, Qt::SolidLine));

	if (absoluteintensity) {
		simpletext = scene->addSimpleText(QString::number(0), myFont);
	}
	else {
		simpletext = scene->addSimpleText(QString::number(0) + " %", myFont);
	}
	simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2));

	ystep = (h - topmargin - bottommargin) / rulergranularity;
	if (absoluteintensity) {
		if (maxintensity > rulergranularity) {
			for (int i = 1; i < rulergranularity; i++) {
				simpletext = scene->addSimpleText(QString::number((unsigned long long)maxintensity / (unsigned long long)rulergranularity * (unsigned long long)i), myFont);
				simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2 - ystep * i));
			}
		}
	}
	else {
		if (maxintensity > 0) {
			for (int i = 1; i < rulergranularity; i++) {
				simpletext = scene->addSimpleText(QString::number(maxintensity / (double)rulergranularity * (double)i) + " %", myFont);
				simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - simpletext->boundingRect().height() / 2 - ystep * i));
			}
		}
	}

	if (absoluteintensity) {
		simpletext = scene->addSimpleText(QString::number((unsigned long long)maxintensity), myFont);
	}
	else {
		simpletext = scene->addSimpleText(QString::number(maxintensity) + " %", myFont);
	}
	simpletext->setPos(QPointF(leftmargin - 15 - simpletext->boundingRect().width(), h - bottommargin - std::max(h - topmargin - bottommargin, 0) - simpletext->boundingRect().height() / 2));

	// TIC peaks
	if (!hidetic) {

		for (int i = 0; i < ticchromatogram.size(); i++) {

			if ((ticchromatogram[i].mzratio < (double)minscan) || (ticchromatogram[i].mzratio > (double)maxscan)) {
				continue;
			}

			x = getXPositionFromScanID((int)ticchromatogram[i].mzratio, w);

			printintensity = true;
			if (absoluteintensity) {
				y = ticchromatogram[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
				if (ticchromatogram[i].absoluteintensity == 0) {
					printintensity = false;
				}
			}
			else {
				y = ticchromatogram[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
				if (ticchromatogram[i].relativeintensity == 0) {
					printintensity = false;
				}
			}

			if (printintensity) {
				line = scene->addLine(x, h - bottommargin - 2, x, h - bottommargin - std::max((int)y, 2), QPen(Qt::black, 2, Qt::SolidLine));
				peakscount++;
			}

		}

	}

	// EIC peaks
	if (!hideeic) {

		for (int i = 0; i < localeic.size(); i++) {

			if ((localeic[i].mzratio < (double)minscan) || (localeic[i].mzratio > (double)maxscan)) {
				continue;
			}

			x = getXPositionFromScanID((int)localeic[i].mzratio, w);

			printintensity = true;
			if (absoluteintensity) {
				y = localeic[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
				if (localeic[i].absoluteintensity == 0) {
					printintensity = false;
				}
			}
			else {
				y = localeic[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
				if (localeic[i].relativeintensity == 0) {
					printintensity = false;
				}
			}

			if (printintensity) {
				line = scene->addLine(x, h - bottommargin - 2, x, h - bottommargin - std::max((int)y, 2), QPen(Qt::red, 2, Qt::SolidLine));
				line->setZValue(1);
				peakscount++;
			}

		}

	}

	// TIC descriptions
	if (!hidetic && (peakscount < 500)) {

		for (int i = 0; i < ticchromatogram.size(); i++) {

			if ((ticchromatogram[i].mzratio < (double)minscan) || (ticchromatogram[i].mzratio > (double)maxscan)) {
				continue;
			}

			x = getXPositionFromScanID((int)ticchromatogram[i].mzratio, w);

			printintensity = true;
			if (absoluteintensity) {
				y = ticchromatogram[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
				if (ticchromatogram[i].absoluteintensity == 0) {
					printintensity = false;
				}
			}
			else {
				y = ticchromatogram[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
				if (ticchromatogram[i].relativeintensity == 0) {
					printintensity = false;
				}
			}

			if (printintensity) {
			
				if (retentiontime) {
					simpletext = scene->addSimpleText(QString::number(rtimes[i]) + " ", myFont);
				}
				else {
					simpletext = scene->addSimpleText(QString::number((int)ticchromatogram[i].mzratio) + " ", myFont);
				}
				tx = x - 2;
				ty = h - bottommargin - std::max((int)y, 2) - simpletext->boundingRect().height() - 1 - 4;
				simpletext->setPos(tx, ty);
				simpletext->setBrush(Qt::black);

				if (scene->collidingItems(simpletext, Qt::IntersectsItemBoundingRect).size() > 0) {
					scene->removeItem(simpletext);
				}

			}

		}

	}

	// EIC descriptions
	if (!hideeic && (peakscount < 500)) {

		for (int i = 0; i < localeic.size(); i++) {

			if ((localeic[i].mzratio < (double)minscan) || (localeic[i].mzratio >(double)maxscan)) {
				continue;
			}

			x = getXPositionFromScanID((int)localeic[i].mzratio, w);

			printintensity = true;
			if (absoluteintensity) {
				y = localeic[i].absoluteintensity / maxintensity * (h - topmargin - bottommargin);
				if (localeic[i].absoluteintensity == 0) {
					printintensity = false;
				}
			}
			else {
				y = localeic[i].relativeintensity / maxintensity * (h - topmargin - bottommargin);
				if (localeic[i].relativeintensity == 0) {
					printintensity = false;
				}
			}

			if (printintensity) {

				QList<QGraphicsItem *> hiddenitems;
				qreal tx, ty, tw, th, sumh;

				vector<string> hits;
				string tmplong;
				string tmpshort;
				size_t pos;
				
				hits.clear();

				if (!hidelabels) {
					tmplong = localeic[i].description.c_str();
					pos = tmplong.find("<br/>");
					while (pos != string::npos) {
						tmpshort = tmplong.substr(0, pos);
						hits.push_back(tmpshort);
						tmplong = tmplong.substr(pos + 5);
						pos = tmplong.find("<br/>");
					}
					if (tmplong.size() > 0) {
						hits.push_back(tmplong);
					}
					sort(hits.rbegin(), hits.rend());
				}

				if (retentiontime) {
					tmpshort = QString::number(rtimes[i]).toStdString();
				}
				else {
					tmpshort = QString::number((int)localeic[i].mzratio).toStdString();
				}

				hits.push_back(tmpshort);

				hiddenitems.clear();
				sumh = 0;
				for (vector<string>::reverse_iterator rit = hits.rbegin(); rit != hits.rend(); ++rit) {
					text = scene->addText("");
					text->setDefaultTextColor(QColor(Qt::red));
					text->setFont(myFont);
					text->setTextInteractionFlags(Qt::TextBrowserInteraction);
					text->setOpenExternalLinks(true);
					text->setHtml(rit->c_str());

					tw = text->boundingRect().width();
					th = text->boundingRect().height();
					sumh += th + 1;
					tx = x - 2 - 4;
					ty = h - bottommargin - std::max((int)y, 2) - sumh - 4;
					text->setPos(tx, ty);
					text->setZValue(2);

					hiddenitems.append(text);

					if (scene->items(tx, ty, tw, th, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder).size() > 1) {
						for (int k = 0; k < (int)hiddenitems.size(); k++) {
							scene->removeItem(hiddenitems[k]);
						}
						break;
					}

				}

			}

		}

	}

	// estimate chromatographic profile peaks
	if (rawdatastate && !hideeic && (maxscan > minscan - 1)) {

		double ynorm = (double)(h - topmargin - bottommargin);

		if (peakshape > 0) {
			calculateExponentialParameters(localeic, rtimes, 1, absoluteintensity, false, fronting, ynorm, bases, taus);
		}

		for (int i = 0; i < (int)nys.size(); i++) {
			nys[i] = nys[i] - ((double)minscan - 1.0);
			nys[i] /= (double)maxscan - ((double)minscan - 1.0);
			nys[i] *= (double)(w - leftmargin - rightmargin);
			nys[i] += leftmargin;

			sigmas[i] /= (double)maxscan - ((double)minscan - 1.0);
			sigmas[i] *= (double)(w - leftmargin - rightmargin);

			as[i] /= maxintensity;
			as[i] *= (double)(h - topmargin - bottommargin);

			if (peakshape > 0) {
				taus[i] /= (double)maxscan - ((double)minscan - 1.0);
				taus[i] *= (double)(w - leftmargin - rightmargin);
			}
		}

		const int step = 1;

		int xmin = getXPositionFromScanID(minscan - 1.0, w);
		int xmax = getXPositionFromScanID(maxscan, w);

		double relint;
		int yval;

		QPainterPath rpath;

		if (peakshape == 0) {
			for (int i = xmin; i < xmax; i += step) {
				relint = 0;

				for (int j = 0; j < (int)as.size(); j++) {
					relint += computeGaussFunction((double)i, as[j], nys[j], sigmas[j]);
				}

				yval = h - topmargin - bottommargin - (int)relint;

				if (i == xmin) {
					rpath.moveTo(i, yval);
				}
				else {
					rpath.lineTo(i, yval);
				}
			}

			scene->addPath(rpath, QPen(Qt::gray, 1, Qt::SolidLine));
		}

		if (as.size() == taus.size()) {

			QPainterPath rpath2;

			if (peakshape > 0) {
				for (int i = xmin; i < xmax; i += step) {
					relint = 0;

					for (int j = 0; j < (int)as.size(); j++) {
						relint += computeExponentiallyModifiedGaussFunction((double)i, as[j], nys[j], sigmas[j], taus[j], fronting);
					}

					yval = h - topmargin - bottommargin - (int)relint;

					if (i == xmin) {
						rpath2.moveTo(i, yval);
					}
					else {
						rpath2.lineTo(i, yval);
					}
				}

				scene->addPath(rpath2, QPen(Qt::gray, 1, Qt::SolidLine));
			}

			//if (taus.size() == bases.size()) {

			//	QPainterPath rpath3;

			//	for (int i = xmin; i < xmax; i += step) {
			//		relint = 0;

			//		for (int j = 0; j < (int)taus.size(); j++) {
			//			relint += ynorm * taus[j] * computeExponentialFunction((double)i /** log(bases[j])*/, econst, taus[j], fronting);
			//		}

			//		yval = h - topmargin - bottommargin - (int)relint;

			//		if (i == xmin) {
			//			rpath3.moveTo(i, yval);
			//		}
			//		else {
			//			rpath3.lineTo(i, yval);
			//		}
			//	}

			//	scene->addPath(rpath3, QPen(Qt::blue, 1, Qt::SolidLine));

			//}

		}

	}

	scene->removeItem(zoomgroup);
	QRectF brect = scene->itemsBoundingRect();

	if (!hidetic) {
		simpletext = scene->addSimpleText("TIC", myFont);
		simpletext->setPos(brect.x() - 50, brect.y() - 50);
		simpletext->setBrush(Qt::black);
	}

	if (!hideeic) {
		simpletext = scene->addSimpleText("EIC", myFont);
		simpletext->setPos(brect.x() - 50, brect.y() - 35);
		simpletext->setBrush(Qt::red);
	}

	scene->setSceneRect(scene->itemsBoundingRect());
	zoomgroup->setVisible(false);
	scene->addItem(zoomgroup);

}


void cChromatogramWindowWidget::updateZoomGroup() {
	qreal rx1, rx2, ry1, ry2;
	QFont myFont("Arial", 8);

	if (currentx < pressedx) {
		rx1 = currentx;
		rx2 = pressedx;
	}
	else {
		rx1 = pressedx;
		rx2 = currentx;
	}

	if (currenty < pressedy) {
		ry1 = currenty;
		ry2 = pressedy;
	}
	else {
		ry1 = pressedy;
		ry2 = currenty;
	}

	zoomrect->setPen(QPen(Qt::black, 1, Qt::DashLine));
	zoomrect->setRect(QRectF(QPointF(rx1, ry1), QPointF(rx2, ry2)));

	int from = getScanIDFromXPosition((pressedx < currentx) ? pressedx : currentx, origwidth);
	int to = getScanIDFromXPosition((pressedx < currentx) ? currentx : pressedx, origwidth);

	QString qstr;

	if (retentiontime) {
		if ((from > 0) && (from <= (int)rtimes.size()) && (to > 0) && (to <= (int)rtimes.size())) {
			qstr += "Time: ";
			qstr += QString::number(rtimes[from - 1]);
			qstr += "-";
			qstr += QString::number(rtimes[to - 1]);
			qstr += "\ndiff: ";
			qstr += QString::number(rtimes[to - 1] - rtimes[from - 1]);
		}
	}
	else {
		qstr += "Spectrum ID: ";
		qstr += QString::number(from);
		qstr += "-";
		qstr += QString::number(to);
		qstr += "\ndiff: ";
		qstr += QString::number(to - from);
	}

	zoomsimpletextitem->setFont(myFont);
	zoomsimpletextitem->setText(qstr);
	zoomsimpletextitem->setPos(QPointF(pressedx, pressedy - 2));

	zoomgroup->setVisible(true);
}


void cChromatogramWindowWidget::calculateMinMaxScan() {
	if (pressedx < leftmargin) {
		pressedx = leftmargin;
	}

	if (pressedx > origwidth - rightmargin) {
		pressedx = origwidth - rightmargin;
	}

	if (currentx < leftmargin) {
		currentx = leftmargin;
	}

	if (currentx > origwidth - rightmargin) {
		currentx = origwidth - rightmargin;
	}

	int tmpminscan = getScanIDFromXPosition((pressedx < currentx) ? pressedx : currentx, origwidth);
	int tmpmaxscan = getScanIDFromXPosition((pressedx < currentx) ? currentx : pressedx, origwidth);

	if (enablemouseselection) {
		if (tmpminscan != tmpmaxscan) {
			minscan = tmpminscan;
			maxscan = tmpmaxscan;
		}
	}
	else {
		if (pressedx > currentx) {
			minscan = min(minscan + tmpmaxscan - tmpminscan, max(1, ticchromatogram.size()));
			maxscan = min(maxscan + tmpmaxscan - tmpminscan, max(1, ticchromatogram.size()));
		}
		else {
			minscan = max(1, minscan - tmpmaxscan + tmpminscan);
			maxscan = max(1, maxscan - tmpmaxscan + tmpminscan);
		}
	}
}


void cChromatogramWindowWidget::zoomIn() {
	if (currentscale < 32) {
		currentscale += factor;
		setMatrix(originalmatrix);
		scale(currentscale, currentscale);
		verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
	}
}


void cChromatogramWindowWidget::zoomOut() {
	if (currentscale > 1) {
		currentscale -= factor;
		setMatrix(originalmatrix);
		scale(currentscale, currentscale);
		verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
	}
}


void cChromatogramWindowWidget::normalSize() {
	currentscale = 1;
	setMatrix(originalmatrix);
	verticalScrollBar()->setSliderPosition(verticalScrollBar()->maximum());
}


void cChromatogramWindowWidget::retentionTimeStateChanged(bool state) {
	retentiontime = state;
	redrawScene();
}


void cChromatogramWindowWidget::absoluteIntensityStateChanged(bool state) {
	absoluteintensity = state;
	redrawScene();
}


void cChromatogramWindowWidget::rawDataStateChanged(bool state) {
	rawdatastate = state;
	redrawScene();
}


void cChromatogramWindowWidget::setRetentionTimeInterval(double mintime, double maxtime) {
	if (rtimes.size() == 0) {
		return;
	}

	int minid = 0;
	int maxid = 0;

	int size = (int)rtimes.size();
	for (int i = 0; i < size; i++) {
		if (fabs(rtimes[i] - mintime) < fabs(rtimes[minid] - mintime)) {
			minid = i;
		}
		if (fabs(rtimes[i] - maxtime) < fabs(rtimes[maxid] - maxtime)) {
			maxid = i;
		}
	}
	
	setScanIDInterval(minid + 1, maxid + 1);
}


void cChromatogramWindowWidget::resetRetentionTimeInterval() {
	resetScanIDInterval();
}


void cChromatogramWindowWidget::setScanIDInterval(int minid, int maxid) {
	if (maxid < minid) {
		int tmp = maxid;
		maxid = minid;
		minid = tmp;
	}

	minscan = max(1, minid);
	maxscan = min(maxid, ticchromatogram.size());
	emit updateScanIDInterval(minscan, maxscan);

	if ((rtimes.size() > 0) && (rtimes.size() == ticchromatogram.size())) {
		mintime = rtimes[minscan - 1];
		maxtime = rtimes[maxscan - 1];
		emit updateRetentionTimeInterval(mintime, maxtime);
	}

	redrawScene();
}


void cChromatogramWindowWidget::resetScanIDInterval() {
	minscan = 1;
	maxscan = max(1, ticchromatogram.size());
	emit updateScanIDInterval(minscan, maxscan);

	if (rtimes.size() > 0) {
		mintime = rtimes[0];
		maxtime = rtimes[rtimes.size() - 1];
		emit updateRetentionTimeInterval(mintime, maxtime);
	}

	redrawScene();
}


void cChromatogramWindowWidget::enableMouseSelectionTool(bool enable) {
	enablemouseselection = enable;
	pressedx = -1;
	pressedy = -1;

	redrawScene();
}


void cChromatogramWindowWidget::hideTIC(bool state) {
	hidetic = state;
	redrawScene();
}


void cChromatogramWindowWidget::hideEIC(bool state) {
	hideeic = state;
	redrawScene();
}


void cChromatogramWindowWidget::hideLabels(bool state) {
	hidelabels = state;
	redrawScene();
}


void cChromatogramWindowWidget::peakShapeChanged(int shape) {
	peakshape = shape;
	redrawScene();
}

