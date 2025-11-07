// includes and using directives
#include <bits/stdc++.h>
using namespace std;

// trim whitespace from both ends of string
static inline string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// check if a string matches the allowed double format
bool isValidDouble(const string &raw) {
    if (raw.empty()) return false;
    size_t i = 0;
    if (raw[0] == '+' || raw[0] == '-') {
        if (raw.size() == 1) return false; // sign only not allowed
        i = 1;
    }
    bool seenDot = false;
    int digitsBeforeDot = 0;
    int digitsAfterDot = 0;
    for (; i < raw.size(); ++i) {
        char c = raw[i];
        if (c == '.') {
            if (seenDot) return false; // multiple dots
            seenDot = true;
            continue;
        }
        if (c < '0' || c > '9') return false; // invalid char
        if (!seenDot) digitsBeforeDot++;
        else digitsAfterDot++;
    }
    // must have at least one digit before dot
    if (digitsBeforeDot == 0) return false;
    // if dot present, must have at least one digit both sides
    if (seenDot && digitsAfterDot == 0) return false;
    return true;
}

// hold sign integer and fraction parts
struct NumParts {
    char sign; // '+' or '-'
    string intPart; // at least "0"
    string fracPart; // "" if no fraction
};

// parse raw string into sign int part and frac part. strip leading zeros
NumParts parseAndNormalize(const string &raw) {
    string s = raw;
    NumParts out;
    out.sign = '+';
    size_t idx = 0;
    if (s[0] == '+' || s[0] == '-') {
        out.sign = s[0];
        idx = 1;
    }
    size_t dot = s.find('.', idx);
    string intp, fracp;
    if (dot == string::npos) {
        intp = s.substr(idx);
        fracp = "";
    } else {
        intp = s.substr(idx, dot - idx);
        fracp = s.substr(dot + 1);
    }
    // Remove leading zeros from integer part. keep at least one digit
    size_t nonzero = 0;
    while (nonzero < intp.size() && intp[nonzero] == '0') ++nonzero;
    if (nonzero == intp.size()) out.intPart = "0";
    else out.intPart = intp.substr(nonzero);

    out.fracPart = fracp;
    return out;
}

// compare abs val of two numbers ignoring sign
int compareMagnitude(const NumParts &a, const NumParts &b) {
    // compare integer lengths first
    if (a.intPart.size() != b.intPart.size()) {
        return (a.intPart.size() > b.intPart.size()) ? 1 : -1;
    }
    // if len equal, compare int parts
    if (a.intPart != b.intPart) {
        return (a.intPart > b.intPart) ? 1 : -1;
    }
    // Compare frac parts by extending to same length
    size_t la = a.fracPart.size(), lb = b.fracPart.size();
    size_t L = max(la, lb);
    for (size_t i = 0; i < L; ++i) {
        char ca = (i < la) ? a.fracPart[i] : '0';
        char cb = (i < lb) ? b.fracPart[i] : '0';
        if (ca != cb) return (ca > cb) ? 1 : -1;
    }
    return 0;
}

// add two pos numbers digit by digit
NumParts addMagnitude(const NumParts &a_in, const NumParts &b_in) {
    // Make copies to pad
    NumParts a = a_in, b = b_in;
    // align frac parts to same length w padding
    size_t la = a.fracPart.size(), lb = b.fracPart.size();
    size_t Lf = max(la, lb);
    a.fracPart.append(Lf - la, '0');
    b.fracPart.append(Lf - lb, '0');

    // add frac part from right to left
    string frac(Lf, '0');
    int carry = 0;
    for (int i = (int)Lf - 1; i >= 0; --i) {
        int da = a.fracPart[i] - '0';
        int db = b.fracPart[i] - '0';
        int sum = da + db + carry;
        frac[i] = char('0' + (sum % 10));
        carry = sum / 10;
    }

    // add int parts
    string ia = a.intPart, ib = b.intPart;
    // pad left
    size_t lia = ia.size(), lib = ib.size();
    size_t Li = max(lia, lib);
    if (lia < Li) ia = string(Li - lia, '0') + ia;
    if (lib < Li) ib = string(Li - lib, '0') + ib;

    string intRes(Li, '0');
    for (int i = (int)Li - 1; i >= 0; --i) {
        int da = ia[i] - '0';
        int db = ib[i] - '0';
        int sum = da + db + carry;
        intRes[i] = char('0' + (sum % 10));
        carry = sum / 10;
    }
    if (carry) intRes.insert(intRes.begin(), char('0' + carry));

    // remove trailing zeros
    string fracTrimmed = frac;
    while (!fracTrimmed.empty() && fracTrimmed.back() == '0') fracTrimmed.pop_back();

    NumParts out;
    out.sign = '+';
    // remove leading zeros of int
    size_t p = 0;
    while (p < intRes.size() && intRes[p] == '0') ++p;
    out.intPart = (p == intRes.size()) ? "0" : intRes.substr(p);
    out.fracPart = fracTrimmed;
    return out;
}

// subtract smaller pos number from larger one handling borrow
NumParts subtractMagnitude(const NumParts &a_in, const NumParts &b_in) {
    NumParts a = a_in, b = b_in;
    // align frac parts
    size_t la = a.fracPart.size(), lb = b.fracPart.size();
    size_t Lf = max(la, lb);
    a.fracPart.append(Lf - la, '0');
    b.fracPart.append(Lf - lb, '0');

    // subtract frac part right to left
    string frac(Lf, '0');
    int borrow = 0;
    for (int i = (int)Lf - 1; i >= 0; --i) {
        int da = (i < (int)a.fracPart.size()) ? (a.fracPart[i] - '0') : 0;
        int db = (i < (int)b.fracPart.size()) ? (b.fracPart[i] - '0') : 0;
        int val = da - db - borrow;
        if (val < 0) { val += 10; borrow = 1; } else borrow = 0;
        frac[i] = char('0' + val);
    }

    // subtract int parts right to left
    string ia = a.intPart, ib = b.intPart;
    size_t lia = ia.size(), lib = ib.size();
    size_t Li = max(lia, lib);
    if (lia < Li) ia = string(Li - lia, '0') + ia;
    if (lib < Li) ib = string(Li - lib, '0') + ib;

    string intRes(Li, '0');
    for (int i = (int)Li - 1; i >= 0; --i) {
        int da = ia[i] - '0';
        int db = ib[i] - '0';
        int val = da - db - borrow;
        if (val < 0) { val += 10; borrow = 1; } else borrow = 0;
        intRes[i] = char('0' + val);
    }

    // Trim leading zeros in integer and trailing zeros in fraction
    size_t p = 0;
    while (p < intRes.size() && intRes[p] == '0') ++p;
    string intTrim = (p == intRes.size()) ? "0" : intRes.substr(p);

    string fracTrim = frac;
    while (!fracTrim.empty() && fracTrim.back() == '0') fracTrim.pop_back();

    NumParts out;
    out.sign = '+';
    out.intPart = intTrim;
    out.fracPart = fracTrim;
    return out;
}

// handle signs then add or subtract magnitudes
string addStringNumbers(const string &Araw, const string &Braw) {
    NumParts A = parseAndNormalize(Araw);
    NumParts B = parseAndNormalize(Braw);

    // if signs equal add magnitudes
    if (A.sign == B.sign) {
        NumParts res = addMagnitude(A, B);
        res.sign = A.sign;
        // if result is zero return zero
        if (res.intPart == "0" && res.fracPart.empty()) return "0";
        string out;
        if (res.sign == '-') out.push_back('-');
        out += res.intPart;
        if (!res.fracPart.empty()) {
            out.push_back('.');
            out += res.fracPart;
        }
        return out;
    }

    // signs differ so do magnitude subtraction
    int cmp = compareMagnitude(A, B);
    if (cmp == 0) {
        return "0";
    } else if (cmp > 0) {
        // result keeps sign of A
        NumParts res = subtractMagnitude(A, B);
        if (res.intPart == "0" && res.fracPart.empty()) return "0";
        string out;
        if (A.sign == '-') out.push_back('-');
        out += res.intPart;
        if (!res.fracPart.empty()) { out.push_back('.'); out += res.fracPart; }
        return out;
    } else {
        // result keeps sign of B
        NumParts res = subtractMagnitude(B, A);
        if (res.intPart == "0" && res.fracPart.empty()) return "0";
        string out;
        if (B.sign == '-') out.push_back('-');
        out += res.intPart;
        if (!res.fracPart.empty()) { out.push_back('.'); out += res.fracPart; }
        return out;
    }
}

// read lines or a file and print sums line by line
int main(int argc, char *argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    istream *inptr = &cin;
    ifstream infile;
    if (argc >= 2) {
        infile.open(argv[1]);
        if (!infile) {
            cerr << "Error opening file: " << argv[1] << "\n";
            return 1;
        }
        inptr = &infile;
    }
    string line;
    int lineno = 0;
    while (std::getline(*inptr, line)) {
        lineno++;
        line = trim(line);
        if (line.empty()) continue; // skip blank lines
        // tokenize by whitespace
        vector<string> toks;
        {
            istringstream iss(line);
            string t;
            while (iss >> t) toks.push_back(t);
        }
        if (toks.size() < 2) {
            cout << "Line " << lineno << ": invalid format (need two tokens)\n";
            continue;
        }
        // Use first two tokens as nums
        string s1 = toks[0], s2 = toks[1];

        if (!isValidDouble(s1)) {
            cout << "Line " << lineno << ": invalid number \"" << s1 << "\"\n";
            continue;
        }
        if (!isValidDouble(s2)) {
            cout << "Line " << lineno << ": invalid number \"" << s2 << "\"\n";
            continue;
        }
        string result = addStringNumbers(s1, s2);
        cout << result << "\n";
    }

    return 0;
}
