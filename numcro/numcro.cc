#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <tchdb.h>

using namespace std;

const string NOT_ASSIGNED = " ";
const string BLOCK        = "■";

typedef vector<vector<int> > Table;
typedef vector<int> Sequence;

int main(int, char **);
string make_pattern(const Sequence &);

class Dictionary {
private:
  TCHDB *hdb_;

public:
  Dictionary() { }

  void open_dbm(const string &filename) {
    hdb_ = tchdbnew();
    if (!tchdbopen(hdb_, filename.c_str(), HDBOREADER)) {
      int ecode = tchdbecode(hdb_);
      cerr << "[error] open error:%s" << tchdberrmsg(ecode) << endl;
      exit(1);
    }
  }

  void lookup(const string &pattern, vector<string> &words) {
    char *value = tchdbget2(hdb_, pattern.c_str()); 
    if (value == NULL) return;
    string word;
    stringstream ss(value);
    while (ss >> word) {
      words.push_back(word);
    }
    free(value);
  }
};

class Numcro {
private:
  int width_;
  int height_;
  int nkana_;
  Table table_;
  Dictionary dict_;
  vector<string> assign_;

  void _init_table() {
    table_.clear();
    for (int y = 0; y < height_; y++) {
      vector<int> vec;
      for (int x = 0; x < width_; x++) {
        vec.push_back(0);
      }
      table_.push_back(vec);
    }
  }

  void _init_assign() {
    assign_.push_back(BLOCK);
    for (int i =0; i < nkana_; i++) {
      assign_.push_back(NOT_ASSIGNED);
    }
  }

  void _get_sequences(vector<Sequence> &sequences) {
    // row
    for (int y = 0; y < height_; y++) {
      Sequence seq;
      for (int x = 0; x < width_; x++) {
        if (table_[y][x] != 0) {
          seq.push_back(table_[y][x]);
          if (x < width_-1) continue;
        }
        if (seq.size() > 1) {
          Sequence newseq(seq);
          sequences.push_back(newseq);
        }
        seq.clear();
      }
    }
    // column
    for (int x = 0; x < width_; x++) {
      Sequence seq;
      for (int y = 0; y < height_; y++) {
        if (table_[y][x] != 0) {
          seq.push_back(table_[y][x]);
          if (y < height_-1) continue;
        }
        if (seq.size() > 1) {
          Sequence newseq(seq);
          sequences.push_back(newseq);
        }
        seq.clear();
      }
    }
  }

  bool _solve_impl(const vector<Sequence> &sequences) {
    if (sequences.size() == 0) {
      print_table();
      return true;
    } else {
      vector<vector<string> > words_list(sequences.size());
      for (unsigned int i = 0; i < sequences.size(); i++) {
        string pattern = make_pattern(sequences[i]);
        dict_.lookup(pattern, words_list[i]);
      }

      int min_index = -1;
      int min_words = -1;
      for (unsigned int i = 0; i < sequences.size(); i++) {
        if (min_words < 0 || static_cast<int>(words_list[i].size()) < min_words) {
          min_words = words_list[i].size();
          min_index = i;
        }
      }
      Sequence seq = sequences[min_index];
      vector<Sequence> newsequences;
      for (unsigned int i = 0; i < sequences.size(); i++) {
        if (static_cast<int>(i) != min_index) newsequences.push_back(sequences[i]);
      }
      vector<string> words = words_list[min_index];

      for (unsigned int i = 0; i < words.size(); i++) {
        string word = words[i];
        vector<string> oldassign(assign_);
        bool not_match = false;
        for (unsigned int j = 0; j < word.size()/3; j++) {
          string kana = word.substr(j*3, 3);
          if (assign_[seq[j]] == NOT_ASSIGNED) {
            assign_[seq[j]] = kana;
          } else if (assign_[seq[j]] != kana) {
            not_match = true;
            break;
          }
        }
        if (!not_match && _solve_impl(newsequences)) {
          return true;
        }
        assign_ = oldassign;
      }
    }  
    return false;
  }

public:
  Numcro() {
    nkana_ = 0; 
    width_ = 0; 
    height_ = 0; 
  }

  void set_dictionary(const Dictionary &dict) {
    this->dict_ = dict; 
  }

  void read_question(const string &filename) {
    ifstream ifs(filename.c_str());
    string line;
    vector<int> table;
    map<int, string> pairs;
    while (getline(ifs, line)) {
      stringstream ss(line);
      string head;
      ss >> head;
      if (head == "numbers:") {
        ss >> nkana_;
      } else if (head == "width:") {
        ss >> width_;
      } else if (head == "height:") {
        ss >> height_;
      } else if (head == "table:") {
        int n;
        while (ss >> n) table.push_back(n);
      } else if (head == "assign:") {
        string pair;
        while (ss >> pair) {
          size_t pos = pair.find(":", 0);
          string number = pair.substr(0, pos);
          string kana = pair.substr(pos+1, pair.size()-pos);
          pairs[atoi(number.c_str())] = kana;
        }
      }
    } 
    if (nkana_ < 1 || width_ < 1 || height_ < 1
        || (int)table.size() != width_ * height_) {
      cerr << "Illegal format" << endl;
    }

    _init_table();
    _init_assign();
    for (int y = 0; y < height_; y++) {
      for (int x = 0; x < width_; x++) {
        table_[y][x] = table[x + y * width_];
      }
    }
    map<int, string>::iterator it = pairs.begin();
    while (it != pairs.end()) {
      assign_[it->first] = it->second;
      it++;
    }
  }

  bool solve() {
    vector<Sequence> sequences;
    _get_sequences(sequences);
    return _solve_impl(sequences);
  }

  void print_table() {
    stringstream ss;
    // table
    for (int y = 0; y < height_; y++) {
      for (int x = 0; x < width_; x++) {
        int number = table_[y][x];
        if (assign_[number] != NOT_ASSIGNED) {
          ss << assign_[number];
        } else {
          ss.width(2);
          ss << number;
        }
        if (x != width_-1) ss << " ";
      }
      ss << endl;
    }

    // assign
    for (unsigned int i = 1;  i < assign_.size(); i++) {
      ss << i << ":" << assign_[i];
      if (i % 10 == 0) {
        ss << endl;
      } else {
        ss << " ";
      }
    }
    ss << endl;
    cout << ss.str();
  }

};


string make_pattern(const Sequence &seq) {
  map<int, int> seqmap;
  map<int, int>::iterator it;
  stringstream ss;
  int cnt = 1;
  for (unsigned int i = 0; i < seq.size(); i++) {
    it = seqmap.find(seq[i]);
    if (it != seqmap.end()) {
      ss << it->second;
    } else {
      seqmap[seq[i]] = cnt;
      ss << cnt;
      cnt++;
    }
  }
  return ss.str();
}


int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Usage: numcro question" << endl;
    exit(1);
  }
  Dictionary dict;
  dict.open_dbm("dic.hdb");
  Numcro numcro;
  numcro.set_dictionary(dict);
  numcro.read_question(argv[1]);
  cout << "Question:" << endl;
  numcro.print_table();
  cout << endl << "Answer:" << endl;
  numcro.solve();

  return 0;
}

