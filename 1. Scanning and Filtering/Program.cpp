#include<bits/stdc++.h>
using namespace std;

bool is_space(char& c) {
  static vector<char> chars = {NULL, ' ', '\r', '\n', '\t'};
  return (find(chars.begin(), chars.end(), c) != chars.end());
}

string read_file(FILE* file) {
  string s, res;
  res = "";
  while (getline(cin, s)) {
    res += s + "\n";
  }
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

int main() {
  FILE* file_in = freopen("input.c", "r", stdin);
  if (file_in == NULL) {
    cout << "Can't read file.\n";
    return 0;
  }

  string str = read_file(file_in);
  fclose(stdin);

  str = remove_comments(str);
  str = remove_spaces(str);

  freopen("output.txt", "w", stdout);
  cout << str;
  fclose(stdout);

  return 0;
}
