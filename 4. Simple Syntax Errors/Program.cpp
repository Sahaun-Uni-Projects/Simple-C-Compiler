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

void throw_exception(string msg, int line = -1) {
  cerr << "Error ::";
  if (line != -1) cerr << " Line " << line;
  cerr << "\n";

  cerr << msg << "\n";

  exit(-21);
}

/* #endregion */

/* #region Tokens */

class Token {
  virtual void output(ostream& out) {
    out << "[" << val << "]";
  }

protected:
  string type, val;
  int line_no;
  Token(string type = "", string val = "", int line_no = -1)
    : type(type), val(val), line_no(line_no) {}

public:
  static bool check(string s) { return true; }
  string get_type() { return type; }
  string get_val() { return val; }
  int get_line_no() { return line_no; }

  friend ostream& operator << (ostream& out, Token& t) {
    t.output(out);
    return out;
  }
};

class Keyword : public Token {
public:
  Keyword(string val, int line_no = -1) : Token("kw", val, line_no) {}

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
    out << "[" << type << " " << symbol_table_id << "]";
  }

public:
  Identifier(string val, int line_no = -1) : Token("id", val, line_no) {
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
  Separator(string val, int line_no = -1) : Token("sep", val, line_no) {}

  static bool check(string s) {
    static const vector<string> sep = {",", ";", "'", "\""};
    return (find(sep.begin(), sep.end(), s) != sep.end());
  }
};

class Operator : public Token {
public:
  Operator(string val, int line_no = -1) : Token("op", val, line_no) {}

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
  Number(string val, int line_no = -1) : Token("num", val, line_no) {}

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
  Parenthesis(string val, int line_no = -1) : Token("par", val, line_no) {}

  static bool check(string s) {
    static const vector<string> par = {"(", ")", "{", "}", "[", "]"};
    return (find(par.begin(), par.end(), s) != par.end());
  }
};

class Unknown: public Token {
public:
  Unknown(string val, int line_no = -1) : Token("unkn", val, line_no) {}
};

Token* tokenize_string(string str, int line_no) {
  if (Keyword::check(str)     ) return new Keyword(str, line_no);
  if (Identifier::check(str)  ) return new Identifier(str, line_no);
  if (Separator::check(str)   ) return new Separator(str, line_no);
  if (Operator::check(str)    ) return new Operator(str, line_no);
  if (Number::check(str)      ) return new Number(str, line_no);
  if (Parenthesis::check(str) ) return new Parenthesis(str, line_no);
  return new Unknown(str, line_no);
}

/* #endregion */

/* #region Read */

class ProgramLine {
  int id;
  string text;

public:
  ProgramLine(int id = -1, string text = "")
    : id(id), text(text) {}

  int get_id() { return id; }
  string get_text() { return text; }

  vector<Token*> tokenize() {
    stringstream stream(get_text());
    vector<Token*> tokens;
    string lexeme;
    while (stream >> lexeme) {
      tokens.push_back(tokenize_string(lexeme, id));
    }
    return tokens;
  }

  friend ostream& operator << (ostream& out, ProgramLine& l) {
    out << l.id << "\t" << l.text;
    return out;
  }
};

vector<ProgramLine*> read_file(FILE* file) {
  vector<ProgramLine*> res;
  int id = 1;
  string s;
  while (getline(cin, s)) {
    res.push_back(new ProgramLine(id, s));
    ++id;
  }
  return res;
}

string read_file_string(FILE* file) {
  vector<ProgramLine*> lines = read_file(file);
  string res = "";
  for (ProgramLine* line : lines) {
    res += line->get_text() + "\n";
  }
  return res;
}

/* #endregion */

/* #region Lexical Analysis */

bool is_delimiter(string& str) {
  if (Operator::check(str)    ) return true;
  if (Separator::check(str)   ) return true;
  if (Parenthesis::check(str) ) return true;
  return false;
}

vector<ProgramLine*> lexical_analysis(vector<ProgramLine*> lines) {
  static const int MAX_DEL_SIZE = 3;

  vector<ProgramLine*> res;
  string s, r, t;
  int n, i, j, len;

  for (ProgramLine* line : lines) {
    s = line->get_text();
    n = s.size();
    r = "";

    for (i = 0; i < n; ++i) {
      if (s[i] == ' ') continue;
      for (j = i; j < n; ++j) {
        len = 0;
        t = s.substr(j, min(n-1, j+MAX_DEL_SIZE-1)-j+1);
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
        r += s.substr(i, j-i);
        if (r.back() != ' ') r += ' ';
      }
      if (len > 0) {
        r += s.substr(j, len);
        if (r.back() != ' ') r += ' ';
      }
      i = j+len-1;
    }

    if (!r.empty()) {
      r.pop_back();
      res.push_back(new ProgramLine(line->get_id(), r));
    }
  }

  return res;
}

vector<ProgramLine*> remove_comments(vector<ProgramLine*> lines) {
  vector<ProgramLine*> res;

  string s, r, two;
  int n, i;
  bool single, multi;

  single = false;
  multi = false;
  for (ProgramLine* line : lines) {
    s = line->get_text();
    n = s.size();
    single = false;
    r = "";

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

      r += s[i];
    }

    if (!single && !multi) r += s[i];
    res.push_back(new ProgramLine(line->get_id(), r));
  }

  return res;
}

vector<ProgramLine*> remove_spaces(vector<ProgramLine*> lines) {
  vector<ProgramLine*> res;
  string s, r;
  int n, i;
  bool taken;

  for (ProgramLine* line : lines) {
    s = line->get_text();
    n = s.size();
    taken = true;
    r = "";

    for (i = 0; i < n; ++i) {
      if (is_space(s[i])) {
        if (!taken) {
          r += " ";
          taken = true;
        }
      } else {
        r += s[i];
        taken = false;
      }
    }

    res.push_back(new ProgramLine(line->get_id(), r));
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
        } else {
          throw_exception("Undeclared variable \"" + curr->get_val() + "\" found.", curr->get_line_no());
        }
        goto Continue;
      }

      if (stid != -1) {
          throw_exception("Variable \"" + curr->get_val() + "\" has been declared already.", curr->get_line_no());
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

/* #region Errors */

void check_error_parens(vector<Token*> tokens) {
  static map<string,int> par_to_id = {
    {"(", 0}, {")", 1},
    {"{", 2}, {"}", 3},
    {"[", 4}, {"]", 5},
  };
  static map<int,string> id_to_par = {
    {0, "("}, {1, ")"},
    {2, "{"}, {3, "}"},
    {4, "["}, {5, "]"},
  };

  int i, par, ind, line_no;
  stack<int> st[3];

  for (Token* t : tokens) {
    if (t->get_type() != "par") continue;
    par = par_to_id[t->get_val()];
    ind = par/2;
    line_no = t->get_line_no();
    if (par & 1) {
      if (st[ind].empty()) {
        throw_exception("Misplaced \"" + t->get_val() + "\" found.", t->get_line_no());
      }
      st[ind].pop();
    } else {
      st[ind].push(t->get_line_no());
    }
  }

  for (i = 0; i < 3; ++i) {
    if (!st[i].empty()) {
      throw_exception("Misplaced \"" + id_to_par[i*2] + "\" found.", st[i].top());
    }
  }
}

void check_error_if_else(vector<Token*> tokens) {
  int cnt = 0;

  for (Token* t : tokens) {
    if (t->get_type() != "kw") continue;

    if (t->get_val() == "if") {
      ++cnt;
    } else if (t->get_val() == "else") {
      --cnt;
      if (cnt < 0) {
        throw_exception("Unmatched \"else\" found.", t->get_line_no());
      }
    }
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

  vector<ProgramLine*> lines = read_file(file_in);
  fclose(stdin);

  lines = remove_comments(lines);
  lines = remove_spaces(lines);
  lines = lexical_analysis(lines);

  vector<Token*> tokens;
  for (ProgramLine* line : lines) {
    vector<Token*> curr = line->tokenize();
    tokens.insert(tokens.end(), curr.begin(), curr.end());
  }
  check_error_parens(tokens);
  check_error_if_else(tokens);

  populate_symbol_table(tokens);

  return 0;
}
