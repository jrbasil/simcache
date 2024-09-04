#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <string>
#include <cmath>
using namespace std;

typedef string hex_str;
typedef string bin_str;

struct line
{
  size_t num = 0;
  size_t set = 0;
  size_t tag = 0;
  size_t ctr = 0;
};

size_t bin_as_dec(bin_str s) // Credit: https://stackoverflow.com/a/13567425
{
  return stoi(s, 0, 2);
}

bin_str hex_as_bin(hex_str s) // Credit: https://stackoverflow.com/a/18311086
{
  stringstream ss;
  ss << hex << s;
  size_t n;
  ss >> n;
  bitset<32> b(n);
  return b.to_string();
}

double simulate_cache
(size_t cache_size, size_t line_size, size_t lines_per_set, bool update_on_use)
{
  if (cache_size % 2 || line_size % 2 || line_size < 4) return 0;

  double hit = 0, counter = 0;

  size_t num_lines = round(cache_size / line_size);
  size_t num_sets = round(num_lines / lines_per_set);

  if (num_lines <= 0 || num_sets <= 0) return 0;

  size_t offset_field_width = round(log(line_size) / log(2));
  size_t line_field_width = round(log(num_lines) / log(2));
  size_t set_field_width = round(log(num_sets) / log(2));

  vector<vector<line> > sets(num_sets);
  vector<line> lines(lines_per_set);
  for (int i = 0; i < num_sets; i++) sets.at(i) = lines;

  enum cache_type { DIRECT_MAPPED, SET_ASSOCIATIVE, FULLY_ASSOCIATIVE };
  cache_type type;

  if (num_sets == 1) type = FULLY_ASSOCIATIVE; else
  if (num_sets == num_lines) type = DIRECT_MAPPED; else
  type = SET_ASSOCIATIVE;

  ifstream infile("gcc.trace");

  char op_type = ' ';
  hex_str hex_address = "";
  size_t num_bytes = 0;
  while (infile >> op_type >> hex_address >> num_bytes)
  {
    bin_str bin_address = hex_as_bin(hex_address);
    size_t offset_field_index = bin_address.size() - offset_field_width;

    bin_str bin_base = bin_address.substr(0, offset_field_index);
    size_t base_end_index = bin_base.size();

    line l;
    bool line_found = 0;

    counter++;
    l.ctr = counter;

    if (type == DIRECT_MAPPED)
    {
      size_t line_field_index = (base_end_index - line_field_width);
      l.tag = bin_as_dec(bin_base.substr(0, line_field_index));
      l.num = bin_as_dec(bin_base.substr(line_field_index, base_end_index));
      l.set = l.num;
      if (l.num >= num_lines) continue;

      if (sets.at(l.num).at(0).tag == l.tag)
      {
        line_found = 1;
        hit++;
        if (update_on_use) sets.at(l.num).at(0).ctr = counter;
      }
      if (line_found) continue;

      sets.at(l.num).at(0) = l;
    }
    else

    if (type == SET_ASSOCIATIVE)
    {
      size_t set_field_index = (base_end_index - set_field_width);
      l.tag = bin_as_dec(bin_base.substr(0, set_field_index));
      l.set = bin_as_dec(bin_base.substr(set_field_index, base_end_index));
      if (l.set >= num_sets) continue;

      vector<line> lines = sets.at(l.set);
      for (int i = 0; i < lines_per_set; i++)
      {
        if (lines.at(i).tag == l.tag)
        {
          line_found = 1;
          l.num = (l.set * lines_per_set + i);
          hit++;
          if (update_on_use) sets.at(l.set).at(i).ctr = counter;
          break;
        }
      }
      if (line_found) continue;

      for (int i = 0; i < lines_per_set; i++)
      {
          if (lines.at(i).tag == 0)
          {
            line_found = 1;
            l.num = (l.set * lines_per_set + i);
            sets.at(l.set).at(i) = l;
            break;
          }
      }
      if (line_found) continue;

      size_t initial_ctr = sets.at(l.set).at(0).ctr;
      size_t replace_index = 0;
      for (int i = 1; i < lines_per_set; i++)
      {
        size_t current_ctr = sets.at(l.set).at(i).ctr;
        if (initial_ctr > current_ctr)
        {
          initial_ctr = current_ctr;
          replace_index = i;
        }
      }
      l.num = (l.set * lines_per_set + replace_index);
      sets.at(l.set).at(replace_index) = l;
    }
    else

    if (type == FULLY_ASSOCIATIVE)
    {
      l.tag = bin_as_dec(bin_base);

      for (size_t i = 0; i < num_lines; i++)
        if (sets.at(0).at(i).tag == l.tag)
        {
          line_found = 1;
          l.num = i;
          hit++;
          if (update_on_use) sets.at(0).at(i).ctr = counter;
          break;
        }
      if (line_found) continue;

      for (size_t i = 0; i < num_lines; i++)
        if (sets.at(0).at(i).tag == 0)
        {
          line_found = 1;
          l.num = i;
          sets.at(0).at(i) = l;
          break;
        }
      if (line_found) continue;

      size_t initial_ctr = sets.at(0).at(0).ctr;
      size_t replace_index = 0;
      for (size_t i = 1; i < num_lines; i++)
      {
        size_t current_ctr = sets.at(0).at(i).ctr;
        if (initial_ctr > current_ctr)
        {
          initial_ctr = current_ctr;
          replace_index = i;
        }
      }
      l.num = replace_index;
      sets.at(0).at(replace_index) = l;
    }
  }

  return (hit / counter);
}

int main()
{
  size_t cache_size = 0, line_size = 0, lines_per_set = 0;
  bool update_on_use = 0;

  cin >> cache_size >> line_size >> lines_per_set >> update_on_use;
  cout << simulate_cache(cache_size, line_size, lines_per_set, update_on_use);

  return 0;
}
