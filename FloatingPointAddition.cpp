#include <iostream>
#include <bitset>
#include <fstream>
#include <vector>

using namespace std;
#define NaN "01111111100000000000000000000001"
#define Denormal "00000000000000000000000000000001"
#define Zero "00000000000000000000000000000000"
#define Inf "01111111100000000000000000000000"
#define InfN "11111111100000000000000000000000"

struct FloatingPointAddition
{
    bool denormal_flag = false;
    bool zero_flag = false;
    bool Flow_flag = false;
    bool residue1_flag = false;
    int total_cycles = 0;

    string add_result, roundOff_result;

    string bin_float_addition(string operand1, string operand2)
    {
        string sign1 = operand1.substr(0, 1);
        string exponent1 = operand1.substr(1, 8);
        string fraction1 = operand1.substr(9, 23);
        string sign2 = operand2.substr(0, 1);
        string exponent2 = operand2.substr(1, 8);
        string fraction2 = operand2.substr(9, 23);
        string result;

        int grp_val1 = grouping(exponent1, fraction1); //  1 -> Zero, 2-> denormal, 3-> Inf, 4-> NaN, 5-> Normal
        int grp_val2 = grouping(exponent2, fraction2);

        if (grp_val1 == 1 || grp_val2 == 1)
        {
            result = zero_addition(operand1, sign1, grp_val1, operand2, sign2, grp_val2);
        }
        else if (grp_val1 == 2 || grp_val2 == 2)
        {
            result = denormal_addition(operand1, sign1, grp_val1, operand2, sign2, grp_val2);
        }
        else if (grp_val1 == 3 || grp_val2 == 3)
        {
            result = infinity_addition(operand1, sign1, grp_val1, operand2, sign2, grp_val2);
        }
        else if (grp_val1 == 4 || grp_val2 == 4)
        {
            result = NaN_addition(operand1, sign1, grp_val1, operand2, sign2, grp_val2);
        }
        else if (grp_val1 == 5 && grp_val2 == 5)
        {
            result = normal_addition(operand1, sign1, exponent1, fraction1, operand2, sign2, exponent2, fraction2);
        }
        return result;
    }

    string normal_addition(string operand1, string sign1, string exponent1, string fraction1, string operand2, string sign2, string exponent2, string fraction2)
    {
        string sign, fraction, exponent_str, result;
        int expA, expB, exponent;
        string fracA, fracB, signA, signB;
        bitset<8> expb1(exponent1);
        bitset<8> expb2(exponent2);
        int exp1 = (int)(expb1.to_ulong());
        int exp2 = (int)(expb2.to_ulong());
        int gap;

        if (exp1 >= exp2)
        {
            fracA = fraction1;
            expA = exp1;
            signA = sign1;
            fracB = fraction2;
            expB = exp2;
            signB = sign2;
            sign = signA;
            exponent = exp1;
        }
        else
        {
            fracA = fraction2;
            expA = exp2;
            signA = sign2;
            fracB = fraction1;
            expB = exp1;
            signB = sign1;
            sign = signA;
            exponent = exp2;
        }

        gap = expA - expB;

        string significandA = "01" + fracA;
        string significandB = "01" + fracB;

        if (gap > 0)
        {
            total_cycles += 1; // BOX 1.

            for (int i = 0; i < gap; i++)
            {
                significandB = "0" + significandB;
                significandA = significandA + "0";
            }
            expB = expA;
        }

        if (signA != signB)
        {
            significandB = twos_complement(significandB);
        }
        add_result = add(significandA, significandB);
        total_cycles += 1; // BOX 2.

        if ((add_result.at(0) == '1') && (signA != signB))
        {
            add_result = twos_complement(add_result);
            sign = signB;
        }

        // Normailization and Rounding
        if (add_result.at(0) == '1')
        {
            exponent += 1;
            total_cycles += 1; //BOX 3.

            Flow_flag = checkOverflow(exponent);
            if (Flow_flag)
            {
                string add_str = add_result.substr(1, 23);

                if (add_str == "00000000000000000000000")
                {
                    if (exponent >= 255)
                    {
                        if (sign == "0")
                        {
                            cout << "Overflow " << endl;
                            return Inf;
                        }
                        else if (sign == "1")
                        {
                            cout << "Overflow " << endl;
                            return InfN;
                        }
                    }
                }
                else
                {
                    if (exponent == 255)
                    {
                        cout << "Output is NaN " << endl;
                        return NaN;
                    }
                }
            }
            else
            {
                add_result = "0" + add_result;
                roundOff_result = Rounding(add_result);
            }
        }
        else if (add_result.at(0) == '0')
        {
            total_cycles += 1; //BOX 3.

            if (add_result.at(1) == '1')
            {
                roundOff_result = Rounding(add_result);
            }
            else if (add_result.at(1) == '0')
            {
                int counter = 0;
                for (int i = 2; i < add_result.length(); i++)
                {
                    if (add_result.at(i) == '0')
                    {
                        counter += 1;
                        zero_flag = true;
                        continue;
                    }
                    else if (add_result.at(i) == '1')
                    {
                        counter += 1;
                        zero_flag = false;
                        break;
                    }
                }
                if (zero_flag)
                {
                    return Zero;
                }
                exponent = exponent - counter;
                add_result.erase(0, counter);

                for (int i = 0; i < counter; i++)
                {
                    add_result = add_result + "0";
                }

                Flow_flag = checkUnderflow(exponent);
                if (Flow_flag)
                {
                    if (denormal_flag)
                    {
                        cout << "Output is Denormal " << endl;
                        roundOff_result = Rounding(add_result);
                    }
                    else
                    {
                        return "Underflow";
                    }
                }
                else
                {
                    roundOff_result = Rounding(add_result);
                }
            }
        }

        if (roundOff_result.at(0) == '1')
        {
            total_cycles += 1; // Noremalization After Rounding
            exponent += 1;
            Flow_flag = checkOverflow(exponent);
            if (Flow_flag)
            {
                string add_str1 = roundOff_result.substr(1, 23);

                if (add_str1 == "00000000000000000000000")
                {
                    if (exponent >= 255)
                    {
                        if (sign == "0")
                        {
                            cout << "Overflow " << endl;
                            return Inf;
                        }
                        else if (sign == "1")
                        {
                            cout << "Overflow " << endl;
                            return InfN;
                        }
                    }
                }
                else
                {
                    if (exponent == 255)
                    {
                        cout << "Output is NaN " << endl;
                        return NaN;
                    }
                }
            }
            else
            {
                bitset<8> exp_bit(exponent);
                exponent_str = exp_bit.to_string();
                fraction = roundOff_result.substr(1, 23);
                result = sign + exponent_str + fraction;
                return result;
            }
        }
        else if (roundOff_result.at(0) == '0')
        {
            if (roundOff_result.at(1) == '1')
            {
                bitset<8> exp_bit(exponent);
                exponent_str = exp_bit.to_string();
                fraction = roundOff_result.substr(2, 23);
                result = sign + exponent_str + fraction;
                return result;
            }
        }

        return Zero;
    }

    string Rounding(string add_result)
    {
        total_cycles += 1; // BOX 4.
        int len = add_result.length();
        if (len <= 25)
        {
            return add_result;
        }
        int len1 = len - 25;
        string significand = add_result.substr(0, 25);
        string residue = add_result.substr(25, len1);
        string residue0 = residue;
        string residue1 = residue0.erase(0, 1);
        char lsb_significand = significand.at(24);
        char msb_residue = residue.at(0);
        char sticky = '0';
        string rounding_result = "";

        if (residue == "")
        {
            return significand;
        }
        if (residue1 == "")
        {
            sticky = '0';
            residue1_flag = true;
        }

        if (residue1_flag == false)
        {
            for (int i = 0; i < residue1.length(); i++)
            {
                if (residue1.at(i) == '1')
                {
                    sticky = '1';
                    break;
                }
            }
        }

        if (msb_residue & sticky == '1')
        {
            rounding_result = add(significand, "0000000000000000000000001");
        }
        else if (msb_residue & flip1(sticky) == '1')
        {
            if (lsb_significand == '1')
            {
                rounding_result = add(significand, "0000000000000000000000001");
            }
            else if (lsb_significand == '0')
            {
                rounding_result = significand;
            }
        }
        else
        {
            rounding_result = significand;
        }
        return rounding_result;
    }

    bool checkUnderflow(int exponent)
    {
        if (exponent < 0)
        {
            return 1;
        }
        else if (exponent == 0)
        {
            denormal_flag = true;
            return 1;
        }
        else
            return 0;
    }

    bool checkOverflow(int exponent)
    {
        if (exponent >= 255)
        {
            return 1;
        }
        else
            return 0;
    }

    string add(string significandA, string significandB)
    {
        string result;
        int length = significandB.length();
        int carryover = 0;

        for (int i = length - 1; i >= 0; i--)
        {
            int first_bit = significandA.at(i) - '0';
            int second_bit = significandB.at(i) - '0';

            int sum = (first_bit ^ second_bit ^ carryover) + '0'; // XOR
            result = (char)sum + result;
            carryover = (first_bit & second_bit) | (second_bit & carryover) | (first_bit & carryover);
        }

        return result;
    }

    string twos_complement(string significandB)
    {

        int len = significandB.length();
        string one, two;
        one = two = "";

        for (int i = 0; i < len; i++)
        {
            one = one + flip(significandB.at(i));
        }
        two = one;

        for (int i = len - 1; i >= 0; i--)
        {
            if (one.at(i) == '1')
            {
                two.at(i) = '0';
            }
            else
            {
                two.at(i) = '1';
                break;
            }
        }
        return two;
    }

    string flip(char bit)
    {

        if (bit == '0')
        {
            return "1";
        }
        else
        {
            return "0";
        }
    }
    char flip1(char bit)
    {

        if (bit == '0')
        {
            return '1';
        }
        else
        {
            return '0';
        }
    }

    string zero_addition(string operand1, string sign1, int grp_val1, string operand2, string sign2, int grp_val2)
    {
        total_cycles += 1;

        if (grp_val1 == 1 && grp_val2 != 1)
        {
            if (grp_val2 == 2)
            {
                cout << "Input is Denormal " << endl;
                return Denormal;
            }
            else if (grp_val2 == 3)
            {
                cout << "Input is Infinity " << endl;
            }
            else if (grp_val2 == 4)
            {
                cout << "Input is NaN " << endl;
                return NaN;
            }
            return operand2;
        }
        else if (grp_val1 != 1 && grp_val2 == 1)
        {
            if (grp_val1 == 2)
            {
                cout << "Input is Denormal " << endl;
                return Denormal;
            }
            else if (grp_val1 == 3)
            {
                cout << "Input is Infinity " << endl;
            }
            else if (grp_val1 == 4)
            {
                cout << "Input is NaN " << endl;
                return NaN;
            }
            return operand1;
        }
        else if (grp_val1 == 1 && grp_val2 == 1)
        {
            if (sign1 == "1" && sign2 == "1")
            {
                return "10000000000000000000000000000000";
            }
            else
            {
                return Zero;
            }
        }
        else
        {
            return Zero;
        }
    }

    string denormal_addition(string operand1, string sign1, int grp_val1, string operand2, string sign2, int grp_val2)
    {
        total_cycles += 1;

        if ((grp_val1 == 2 && grp_val2 == 2) || (grp_val1 == 2 && grp_val2 == 5) || (grp_val1 == 5 && grp_val2 == 2))
        {
            cout << "Input is Denormal " << endl;
            return Denormal;
        }
        else if (grp_val1 == 2 && grp_val2 == 3)
        {
            cout << "Input is Infinity " << endl;
            return operand2;
        }
        else if (grp_val1 == 3 && grp_val2 == 2)
        {
            cout << "Input is Infinity " << endl;
            return operand1;
        }
        else if ((grp_val1 == 2 && grp_val2 == 4) || (grp_val1 == 4 && grp_val2 == 2))
        {
            cout << "Input is NaN " << endl;
            return NaN;
        }
        else
        {
            return Denormal;
        }
    }

    string infinity_addition(string operand1, string sign1, int grp_val1, string operand2, string sign2, int grp_val2)
    {
        total_cycles += 1;

        if (grp_val1 == 3 && (grp_val2 == 1 || grp_val2 == 5))
        {
            cout << "Input is Infinity " << endl;
            return operand1;
        }
        else if ((grp_val1 == 1 || grp_val1 == 5) && grp_val2 == 3)
        {
            cout << "Input is Infinity " << endl;
            return operand2;
        }
        else if (grp_val1 == 3 && grp_val2 == 3)
        {
            if (sign1 == sign2)
            {
                cout << "Input is Infinity " << endl;
                return operand1;
            }
            else
            {
                cout << " Output is NaN " << endl;
                return NaN;
            }
        }
        else if ((grp_val1 == 3 && grp_val2 == 4) || (grp_val1 == 4 && grp_val2 == 3))
        {
            cout << "Input is NaN " << endl;
            return NaN;
        }
        else
        {
            return Zero;
        }
    }

    string NaN_addition(string operand1, string sign1, int grp_val1, string operand2, string sign2, int grp_val2)
    {
        total_cycles += 1;
        cout << "Input is NaN " << endl;
        return NaN;
    }

    int grouping(string exponent, string fraction)
    {
        total_cycles += 1;

        if (exponent == "00000000")
        {
            if (fraction == "00000000000000000000000")
            {
                return 1;
            }
            else
            {
                return 2;
            }
        }
        else if (exponent == "11111111")
        {
            if (fraction == "00000000000000000000000")
            {
                return 3;
            }
            else
            {
                return 4;
            }
        }
        else
        {
            return 5;
        }
    }
};

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cout << "NO INPUT FILE PROVIDED!!\n";
        exit(1);
    }
    ifstream file;
    file.open(argv[1]);
    if (!file.is_open())
    {
        cout << "FILE COULD NOT OPENED\n";
        exit(2);
    }
    string str;

    while (getline(file, str))
    {
        string op1 = str.substr(0, 32);
        string op2 = str.substr(33, 32);
        FloatingPointAddition fp = FloatingPointAddition();
        string res = fp.bin_float_addition(op1, op2);
        cout << res << endl;
        //cout << res << " " << fp.total_cycles - 1 << endl;
    }
    return 0;
}

/*int main()
{
    FloatingPointAddition fp = FloatingPointAddition();
    string str = fp.bin_float_addition("00111111110000000000000000000000", "00111111110000000000000000000000");
    cout << "output string is " << str << endl;
    cout << "total cycles: " << fp.total_cycles - 1 << endl;
    return 0;
}*/