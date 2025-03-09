#include "GenerateMotorCore.hpp"

#include "../../Vvvf/InternalMath.hpp"

namespace VvvfSimulator::Generation::Motor::GenerateMotorCore
{
	using namespace NAMESPACE_VVVF::InternalMath;
	
	void Motor::updateParameter(const WaveValues &voltage, double theta)
	{
		m_parameter.sitamr = theta;
		for (int i = 0; i < 3; i++) m_parameter.Uabc[i] = 110.0 * voltage[i];

		m_parameter.Udq0[0] = Functions::cosine(m_parameter.sitamr) * m_parameter.Uabc[0] + Functions::cosine(m_parameter.sitamr - m_2PI / 3.0) * m_parameter.Uabc[1] + Functions::cosine(m_parameter.sitamr + m_2PI / 3.0) * m_parameter.Uabc[2];
		m_parameter.Udq0[1] = -(Functions::sine(m_parameter.sitamr) * m_parameter.Uabc[0] + Functions::sine(m_parameter.sitamr - m_2PI / 3.0) * m_parameter.Uabc[1] + Functions::sine(m_parameter.sitamr + m_2PI / 3.0) * m_parameter.Uabc[2]);
		
		// Parameter Calculation
		{
			double u_sm = m_parameter.Udq0[0];
			double u_st = m_parameter.Udq0[1];
			double T_e;
			double T_L = m_parameter.TL;
			double R_s = specification.R_s;
			double R_r = specification.R_r;
			double L_m = specification.L_m;
			double L_r = specification.L_r;
			double L_s = specification.L_s;
			double i_d = m_parameter.Idq0[0];
			double i_q = m_parameter.Idq0[1];
			double w_r = m_parameter.w_r;
			double FLUX = m_parameter.r_Flux;
			double NP = specification.NP;
			double wsl = m_parameter.wsl;
			double rsita = m_parameter.sita_r;
			double wmr;
			double sitamr = m_parameter.sitamr;
			// Other
			double i_m1 = m_parameter.i_m1;
			//Rotor electrical constant
			double T_r = L_r / R_r;
			double temp2 = L_m * L_m;
			double temp1;
			double eta = 1 - temp2 / (L_s * L_r);
			double temp;

			temp1 = eta * L_s;
			temp = eta * L_s * L_r * T_r;

			// Excitation current equation
			i_d = ((-R_s / temp1 - temp2 / temp) * i_d + L_m / temp * FLUX + u_sm / temp1) / samplingFrequency + i_d; 
                
			// Torque-current equation
			i_q = ((-R_s / temp1 - temp2 / temp) * i_q - FLUX * (L_m * w_r / (temp1 * L_r)) + u_st / temp1) / samplingFrequency + i_q;
                
			//转子磁链方程
			//FLUX = i_mL_m/(T_rs +1);///一阶惯性环节 双线性变换推导出
                
			//Rotor flux linkage is the first - order inertia of excitation current
			FLUX = L_m / (temp + 1) * (i_d + i_m1) - (1 - temp) * FLUX / (temp + 1); 
                
			i_m1 = i_d;
			T_e = NP * i_q * FLUX * L_m / L_r; /*Moment equation*/
			if (FLUX != 0)
				wsl = L_m * i_q / (T_r * FLUX); /*The slip equation may be divided by 0 here*/
			if ((std::abs(T_e - T_L) < specification.STATICF) && (w_r == 0)) /*Simulating static friction*/
				w_r = 0;
			else /*Simulated running equation of motion*/
				w_r = NP * ((T_e - T_L - (specification.DAMPING * w_r / NP)) / specification.INERTIA) / samplingFrequency + w_r;

			rsita += w_r / samplingFrequency;
			wmr = (wsl + w_r) / samplingFrequency;
			/*Input rotor position*/
			sitamr += wmr;
			sitamr = std::fmod(sitamr, m_2PI);
			if (sitamr < 0)
				sitamr += m_2PI;
			/*Rotor position obtained by integration*/
			rsita = std::fmod(rsita, m_2PI);
			if (rsita < 0)
				rsita += m_2PI;

			m_parameter.w_mr = wmr;
			m_parameter.sitamr = sitamr;
			m_parameter.sita_r = rsita;
			m_parameter.Idq0[0] = i_d;
			m_parameter.Idq0[1] = i_q;
			m_parameter.w_r = w_r;
			m_parameter.r_Flux = FLUX;
			m_parameter.wsl = wsl;
			m_parameter.Te = T_e;
			m_parameter.i_m1 = i_m1;
		}

		const double al = m_parameter.Idq0[0] * Functions::cosine(m_parameter.sitamr) - m_parameter.Idq0[1] * Functions::sine(m_parameter.sitamr);
		const double be = m_parameter.Idq0[1] * Functions::cosine(m_parameter.sitamr) + m_parameter.Idq0[0] * Functions::sine(m_parameter.sitamr);

		m_parameter.Iabc[0] = std::sqrt(1.5) * al;
		m_parameter.Iabc[1] = std::sqrt(1.5) *  (be * m_SQRT3_2 - al * 0.5);
		m_parameter.Iabc[2] = std::sqrt(1.5) * -(be * m_SQRT3_2 + al * 0.5);

		for (int i = 0; i < 3; i++)
		{
			m_parameter.diffIdq0[i] = m_parameter.Idq0[i] - m_parameter.preIdq0[i];
			m_parameter.preIdq0[i] = m_parameter.Idq0[i];
		}
	}
}