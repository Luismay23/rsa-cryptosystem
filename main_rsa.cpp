#include <iostream>
#include <gmpxx.h>
#include <ctime>

using namespace std;

std::ostream& operator<<(std::ostream& out, const mpz_class& num) {
	out << num.get_str();
	return out;
}
std::istream& operator>>(std::istream& in, mpz_class& num) {
    std::string input;
    in >> input;
    if (!input.empty() && std::all_of(input.begin(), input.end(), ::isdigit)) {
        num.set_str(input, 10); 
    } 
    return in;
}

bool millerRabinTest(mpz_class& num, int rounds) {
	if (num <= 1) return false;

	mpz_class r = num - 1;
	int u = 0;
	while (mpz_even_p(r.get_mpz_t())) {
		r /= 2;
		u++;
	}

	gmp_randclass rand_state(gmp_randinit_default);

	for (int i = 0; i < rounds; i++) {
		mpz_class a = rand_state.get_z_range(num - 2) + 2;

		mpz_class z;
		mpz_powm(z.get_mpz_t(), a.get_mpz_t(), r.get_mpz_t(), num.get_mpz_t());

		if (z == 1 || z == num - 1) {
			continue;
		}

		bool composite = true;
		for (int j = 0; j < u - 1; j++) {
			mpz_class exponent = 2;
			mpz_powm(z.get_mpz_t(), z.get_mpz_t(), exponent.get_mpz_t(), num.get_mpz_t());
			if (z == 1) {
				return false;
			}
			if (z == num - 1) {
				composite = false;
				break;
			}
		}
		if (composite) return false;
	}

	return true;
}

mpz_class EA(mpz_class a, mpz_class b) {
	if(b==0) {
		return a;
	}
	mpz_class r;
	mpz_mod(r.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
	return EA(b, r);
}

mpz_class EEA(mpz_class e, mpz_class totientN) {
	mpz_class s0 = totientN, s1 = e;
	mpz_class t0 = 0, t1 = 1;

	while (s1 != 0) {
		mpz_class q = s0 / s1;
		mpz_class s = s0 - q * s1;
		mpz_class t = t0 - q * t1;

		s0 = s1;
		s1 = s;
		t0 = t1;
		t1 = t;
	}

	if (s0 != 1) {
		cout<<"There is no inverse"<<endl;
		return mpz_class(0);
	}
	if (t0 < 0) {
		t0 += totientN;
	}

	return t0;
}


int main() {
	gmp_randclass rand_state(gmp_randinit_default);
	rand_state.seed(time(0));

	mpz_class p,q,n, pMinus1, qMinus1, totientN, e, d, plaintextX, ciphertextY, decCiphertext;

	while (!millerRabinTest(p, 20)) {
		p = rand_state.get_z_bits(512);
	}
	cout << "Prime number p:\n" << p << endl << endl;

	while (!millerRabinTest(q, 20)) {
		q = rand_state.get_z_bits(512);
	}
	cout << "Prime number q:\n" << q << endl;

	n=p*q;
	pMinus1=p-1;
	qMinus1=q-1;
	totientN=pMinus1*qMinus1;

	cout<<endl;
	cout<<"Totient n :\n"<<totientN<<endl;

	while(EA(e, totientN)!=1) {
		e = rand_state.get_z_range(totientN - 1)+1;
	}

	cout<<endl;
	cout<<"Public key (n,e) :"<<endl;
	cout<<endl;
	cout<<"n :\n"<<n<<endl;

	cout<<endl;
	cout<<"e chosen :\n"<<e<<endl;

	while (mpz_sizeinbase(d.get_mpz_t(), 2) < 154) {
		d = EEA(e, totientN);
	}
	cout<<endl;
	cout<<"Private key (d) :\n"<<d<<endl;

    cout<<endl;
    cout<<"Enter the plaintext :"<<endl;
    std::cin>>plaintextX;
    
    cout<<"The plaintext : "<<plaintextX<<endl;
    mpz_powm(ciphertextY.get_mpz_t(), plaintextX.get_mpz_t(),e.get_mpz_t(), n.get_mpz_t());
    cout<<"The ciphertext :\n"<<ciphertextY<<endl;

    cout<<endl;
    
    mpz_powm(decCiphertext.get_mpz_t(), ciphertextY.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
    cout<<"The decrypted ciphertext : "<<decCiphertext<<endl;
    
	return 0;


}
