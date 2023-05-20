#include<bits/stdc++.h>
using namespace std;

/* #region Macros */

#define SCOPE_GLOBAL "global"

/* #endregion */

/* #region Types */

enum class ID_TYPE {
  VAR, FUNC,
  UNKNOWN,
};

string get_name_from_id_type(ID_TYPE type) {
  switch (type) {
    case ID_TYPE::VAR  : return "var";
    case ID_TYPE::FUNC : return "func";
    default: break;
  }
  return "";
}

ID_TYPE get_id_type_from_name(string name) {
  static map<string,ID_TYPE> mp = {
    {"var", ID_TYPE::VAR},
    {"func", ID_TYPE::FUNC},
  };

  if (mp.find(name) == mp.end()) return ID_TYPE::UNKNOWN;
  return mp[name];
}

enum class DATA_TYPE {
  FLOAT, INT, DOUBLE,
  UNKNOWN,
};

string get_name_from_data_type(DATA_TYPE type) {
  switch (type) {
    case DATA_TYPE::FLOAT  : return "float";
    case DATA_TYPE::INT    : return "int";
    case DATA_TYPE::DOUBLE : return "double";
    default: break;
  }
  return "";
}

DATA_TYPE get_data_type_from_name(string name) {
  static map<string,DATA_TYPE> mp = {
    {"float", DATA_TYPE::FLOAT},
    {"int", DATA_TYPE::INT},
    {"double", DATA_TYPE::DOUBLE},
  };

  if (mp.find(name) == mp.end()) return DATA_TYPE::UNKNOWN;
  return mp[name];
}

/* #endregion */

/* #region Common */

string read_file(FILE* file) {
  string s, res;
  res = "";
  while (getline(cin, s)) {
    res += s + "\n";
  }
  return res;
}

bool is_space(char& c) {
  static const vector<char> chars = {NULL, ' ', '\r', '\n', '\t'};
  return (find(chars.begin(), chars.end(), c) != chars.end());
}

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

void throw_exception(string msg) {
  msg = "Runtime Error ::\n" + msg;
  cerr << msg << "\n";
  throw msg;
}

/* #endregion */

/* #region Tokens */

class Token {
  virtual void output(ostream& out) {
    out << "[" << val << "]";
  }

protected:
  string type, val;
  Token(string type = "", string val = "") : type(type), val(val) {}

public:
  static bool check(string s) { return true; }
  string get_type() { return type; }
  string get_val() { return val; }

  friend ostream& operator << (ostream& out, Token& t) {
    t.output(out);
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

  bool is_data_type() {
    static const vector<string> kw = {
      "char", "double", "float", "int", "long", "short", "void"
    };
    return (find(kw.begin(), kw.end(), val) != kw.end());
  }
};

class Identifier : public Token {
  int symbol_table_id;

  virtual void output(ostream& out) override {
    out << "[" << type << " " << symbol_table_id+1 << "]";
  }

public:
  Identifier(string val) : Token("id", val) {
    symbol_table_id = -1;
  }

  void set_symbol_table_id(int id) {
    symbol_table_id = id;
  }

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

Token* tokenize(string str) {
  if (Keyword::check(str)     ) return new Keyword(str);
  if (Identifier::check(str)  ) return new Identifier(str);
  if (Separator::check(str)   ) return new Separator(str);
  if (Operator::check(str)    ) return new Operator(str);
  if (Number::check(str)      ) return new Number(str);
  if (Parenthesis::check(str) ) return new Parenthesis(str);
  return new Unknown(str);
}

/* #endregion */

/* #region Lexical Analysis */

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

string remove_comments(string& str) {
  stringstream stream(str);
  string s, res, two;
  int n, i;
  bool single, multi;

  single = false;
  multi = false;
  res = "";
  while (getline(stream, s)) {
    n = s.size();
    single = false;

    for (i = 0; i < n-1; ++i) {
      two = s.substr(i, 2);

      if (multi) {
        if (two == "*/") {
            multi = false;
            ++i;
            continue;
        }
        continue;
      }

      if (two == "/*") {
        multi = true;
        ++i;
        continue;
      }

      if (two == "//") {
        single = true;
        break;
      }

      res += s[i];
    }

    if (!single && !multi) res += s[i];
    res += "\n";
  }

  return res;
}

string remove_spaces(string& str) {
  stringstream stream(str);
  string s, res;
  int n, i;
  bool taken;

  taken = false;
  res = "";
  while (getline(stream, s)) {
    n = s.size();

    for (i = 0; i < n; ++i) {
      if (is_space(s[i])) {
        if (!taken) {
          res += " ";
          taken = true;
        }
      } else {
        res += s[i];
        taken = false;
      }
    }

    if (!taken) {
      res += " ";
      taken = true;
    }
  }

  return res;
}

/* #endregion */

/* #region Symbol Table */

class SymbolTable {
  struct Entry {
    int id;
    string name, scope, value;
    ID_TYPE id_type;
    DATA_TYPE data_type;

    Entry(int id, string name, ID_TYPE id_type, DATA_TYPE data_type, string scope, string value)
      : id(id), name(name), id_type(id_type), data_type(data_type), scope(scope), value(value) {}

    friend ostream& operator << (ostream& out, Entry& e) {
      out             << e.id;
      out << "\t\t"   << e.name;
      out << "\t\t"   << get_name_from_id_type(e.id_type);
      out << "\t\t"   << get_name_from_data_type(e.data_type);
      out << "\t\t\t" << e.scope;
      out << "\t\t"   << e.value;
      return out;
    }
  };
  vector<Entry*> entries;

public:
  int insert(string name, ID_TYPE id_type, DATA_TYPE data_type, string scope = SCOPE_GLOBAL, string value = "") {
    int id = entries.size();
    entries.push_back(new Entry(id, name, id_type, data_type, scope, value));
    return id;
  }

  void set_value(int id, string value) {
    if (id >= entries.size()) {
      throw_exception("Symbol table has no entry with id \"" + to_string(id) + "\".");
      return;
    }
    entries[id]->value = value;
  }

  void free() {
    entries.clear();
  }

  int lookup(string name, string scope = SCOPE_GLOBAL) {
    for (Entry* entry : entries) {
      if ((entry->name == name) && (entry->scope == scope)) {
        return entry->id;
      }
    }
    return -1;
  }

  void display() {
    cout << *this;
  }

  friend ostream& operator << (ostream& out, SymbolTable& st) {
    cout << "Id\t\tName\t\tId type\t\tData Type\t\tScope\t\tValue\n";
    for (Entry* entry : st.entries) cout << *entry << "\n";
    return out;
  }
};

/* #endregion */

/* #region Scope Handler */

class ScopeHandler {
  stack<string> scope_stack;

public:
  ScopeHandler() {
    scope_stack.push(SCOPE_GLOBAL);
  }

  void push(string scope) {
    scope_stack.push(scope);
  }

  string pop() {
    string scope = scope_stack.top();
    if (scope == SCOPE_GLOBAL) {
      throw_exception("Global scope should be the topmost scope.");
      return "";
    }
    scope_stack.pop();
    return scope;
  }

  string get_scope() {
    return scope_stack.top();
  }

  stack<string> get_snapshot() {
    return stack<string>(scope_stack);
  }
};

/* #endregion */

/* #region Globals */

namespace global {
  SymbolTable* symbol_table;
  ScopeHandler* scope_handler;
}

void init_globals() {
  global::symbol_table = new SymbolTable();
  global::scope_handler = new ScopeHandler();
}

/* #endregion */

/* #region Symbol Table Helpers */

void populate_symbol_table(vector<Token*> tokens) {
  int n, i;
  ID_TYPE id_type;
  DATA_TYPE data_type;
  string curr_type, name, value, scope;

  n = tokens.size();

  for (i = 1; i < n-1; ++i) {
    Token* prev = tokens[i-1];
    Token* curr = tokens[i];
    Token* next = tokens[i+1];

    scope = global::scope_handler->get_scope();
    curr_type = curr->get_type();

    if (curr_type == "par") {
      if (curr->get_val() == "}") {
        global::scope_handler->pop();
      }
    } else if (curr_type == "id") {
      name = curr->get_val();
      int stid = global::symbol_table->lookup(name, scope);

      if ((prev->get_type() != "kw") || !((Keyword*)prev)->is_data_type()) {
        if (stid == -1) {
          stack<string> tscopes = global::scope_handler->get_snapshot();
          while (!tscopes.empty()) {
            string tscope = tscopes.top(); tscopes.pop();
            stid = global::symbol_table->lookup(name, tscope);
            if (stid != -1) break;
          }
        }
        if (stid != -1) {
          ((Identifier*)curr)->set_symbol_table_id(stid);
        }
        goto Continue;
      }

      data_type = get_data_type_from_name(prev->get_val());
      id_type = (next->get_val() == "(") ? ID_TYPE::FUNC : ID_TYPE::VAR;

      if (id_type == ID_TYPE::FUNC) {
        global::scope_handler->push(name);
      }
      stid = global::symbol_table->insert(name, id_type, data_type, scope);
      ((Identifier*)curr)->set_symbol_table_id(stid);
    } else if (curr_type == "op") {
      if ((curr->get_val() == "=") && (next->get_type() == "num")) {
        int stid = global::symbol_table->lookup(prev->get_val(), scope);
        if (stid != -1) global::symbol_table->set_value(stid, next->get_val());
      }
    }

    Continue:;
  }
}

/* #endregion */

int main() {
  init_globals();

  FILE* file_in = freopen("input.c", "r", stdin);
  if (file_in == NULL) {
    cout << "Can't read file.\n";
    return 0;
  }

  string str = read_file(file_in);
  fclose(stdin);

  str = remove_comments(str);
  str = remove_spaces(str);
  str = lexical_analysis(str);

  string lexeme;
  stringstream stream(str);
  vector<Token*> tokens;
  while (stream >> lexeme) {
    tokens.push_back(tokenize(lexeme));
  }

  populate_symbol_table(tokens);

  freopen("output.txt", "w", stdout);
  cout << *global::symbol_table << "\n";
  for (Token* token : tokens) cout << *token << " ";
  cout << "\n";
  fclose(stdout);

  return 0;
}
