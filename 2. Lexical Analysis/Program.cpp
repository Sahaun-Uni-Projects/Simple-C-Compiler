#include<bits/stdc++.h>
using namespace std;

namespace common {
  string read_file(FILE* file) {
    string s, res;
    res = "";
    while (getline(cin, s)) {
      res += s + "\n";
    }
    return res;
  }
};

bool is_letter_small(char c) {
  return (('a' <= c) && (c <= 'z'));
}

bool is_letter_big(char c) {
  return (('A' <= c) && (c <= 'Z'));
}

bool is_letter(char c) {
  return (is_letter_small(c) || is_letter_big(c));
}

bool is_digit(char c) {
  return (('0' <= c) && (c <= '9'));
}

class Token {
  string type, val;

protected:
  Token(string type = "", string val = "") : type(type), val(val) {}

public:
  static bool check(string s) { return true; }
  string get_type() { return type; }
  string get_val() { return val; }

  friend ostream& operator << (ostream& out, Token& t) {
    out << "[" << t.type << " " << t.val << "]";
    return out;
  }
};

class Keyword : public Token {
public:
  Keyword(string val) : Token("kw", val) {}

  static bool check(string s) {
    static const vector<string> kw = {
      "auto", "break", "case", "char", "const", "continue", "default", "do",
      "double", "else", "enum", "extern", "float", "for", "goto", "if",
      "int", "long", "register", "return", "short", "signed", "sizeof", "static",
      "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
    };
    return (find(kw.begin(), kw.end(), s) != kw.end());
  }
};

class Identifier : public Token {
public:
  Identifier(string val) : Token("id", val) {}

  static bool check(string s) {
    static const vector<int> final_states = {2};
    int state = 1;

    for (char c : s) {
      switch (state) {
        case 1:
          if (c == '_') state = 2;
            else if (is_letter(c)) state = 2;
            else state = 0;
        break;
        case 2:
          if (c == '_') state = 2;
            else if (is_letter(c)) state = 2;
            else if (is_digit(c)) state = 2;
            else state = 0;
        break;
        default: break;
      }
    }

    return (find(final_states.begin(), final_states.end(), state) != final_states.end());
  }
};

class Separator : public Token {
public:
  Separator(string val) : Token("sep", val) {}

  static bool check(string s) {
    static const vector<string> sep = {",", ";", "'", "\""};
    return (find(sep.begin(), sep.end(), s) != sep.end());
  }
};

class Operator : public Token {
public:
  Operator(string val) : Token("op", val) {}

  static bool check(string s) {
    static const vector<vector<string>> op = {
      {"+", "-", "*", "/", "%", ">", "<", "!", "&", "|", "^", "~", "="},
      {"++", "--", "==", "!=", ">=", "<=", "&&", "||", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "&=", "^=", "|="},
      {"<<=", ">>="},
    };
    int i = s.size()-1;
    if ((i < 0) || (i >= op.size())) return false;
    return (find(op[i].begin(), op[i].end(), s) != op[i].end());
  }
};

class Number : public Token {
public:
  Number(string val) : Token("num", val) {}

  static bool check(string s) {
    static const vector<int> final_states = {2, 4};
    int state = 1;

    for (char c : s) {
      switch (state) {
        case 1:
        case 2:
          if (c == '.') state = 3;
            else if (is_digit(c)) state = 2;
            else state = 0;
        break;
        case 3:
        case 4:
          if (is_digit(c)) state = 4;
            else state = 0;
        break;

        default: break;
      }
    }

    return (find(final_states.begin(), final_states.end(), state) != final_states.end());
  }
};

class Parenthesis : public Token {
public:
  Parenthesis(string val) : Token("par", val) {}

  static bool check(string s) {
    static const vector<string> par = {"(", ")", "{", "}", "[", "]"};
    return (find(par.begin(), par.end(), s) != par.end());
  }
};

class Unknown: public Token {
public:
  Unknown(string val) : Token("unkn", val) {}
};

bool is_delimiter(string& str) {
  if (Operator::check(str)    ) return true;
  if (Separator::check(str)   ) return true;
  if (Parenthesis::check(str) ) return true;
  return false;
}

string lexical_analysis(string& str) {
  static const int MAX_DEL_SIZE = 3;

  stringstream stream(str);
  string res, t;
  int n, m, i, j, len;

  res = "";

  while (stream >> str) {
    n = str.size();
    for (i = 0; i < n; ++i) {
      for (j = i; j < n; ++j) {
        len = 0;
        t = str.substr(j, min(n-1, j+MAX_DEL_SIZE-1)-j+1);
        while (!t.empty()) {
          if (is_delimiter(t)) {
            len = t.size();
            break;
          }
          t.pop_back();
        }
        if (len > 0) break;
      }
      if (j != i) {
        res += str.substr(i, j-i) + " ";
      }
      if (len > 0) {
        res += str.substr(j, len) + " ";
      }
      i = j+len-1;
    }
  }
  res.pop_back();

  return res;
}

Token tokenize(string str) {
  if (Keyword::check(str)     ) return Keyword(str);
  if (Identifier::check(str)  ) return Identifier(str);
  if (Separator::check(str)   ) return Separator(str);
  if (Operator::check(str)    ) return Operator(str);
  if (Number::check(str)      ) return Number(str);
  if (Parenthesis::check(str) ) return Parenthesis(str);
  return Unknown(str);
}

int main() {
  FILE* file_in = freopen("input.c", "r", stdin);
  if (file_in == NULL) {
    cout << "Can't read file.\n";
    return 0;
  }

  string str, lexeme;
  stringstream stream;
  vector<Token> tokens;

  str = common::read_file(file_in);
  fclose(stdin);

  str = lexical_analysis(str);

  stream = stringstream(str);
  while (stream >> lexeme) {
    tokens.push_back(tokenize(lexeme));
  }

  freopen("output.txt", "w", stdout);

  for (Token t : tokens) cout << t << " ";
  cout << "\n";

  fclose(stdout);

  return 0;
}
