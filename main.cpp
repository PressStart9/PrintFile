#include <iostream>
#include <fstream>
#include <string>

#include "string.hpp"

enum ErrorCodes { NO_SUCH_FILE, INCORRECT_DELIMITER, NO_SUCH_ARGUMENT, ARGUMENT_NOT_SPECIFIED, FILE_NOT_SPECIFIED,
    NOT_UNSIGNED_INT, INT_OUT_OF_RANGE };

void RaiseError(const ErrorCodes code, const char* note = nullptr) {
  switch (code) {
    case ErrorCodes::NO_SUCH_FILE:
      std::cerr << "There is no such file: " << note << '\n';
      break;
    case ErrorCodes::INCORRECT_DELIMITER:
      std::cerr << "Incorrect delimiter: " << note << '\n';
      break;
    case ErrorCodes::NO_SUCH_ARGUMENT:
      std::cerr << "There is no argument: " << note << '\n';
      break;
    case ErrorCodes::ARGUMENT_NOT_SPECIFIED:
      std::cerr << "You must specify argument instead of " << note << '\n';
      break;
    case ErrorCodes::FILE_NOT_SPECIFIED:
      std::cerr << "File is not specified\n";
      break;
    case ErrorCodes::NOT_UNSIGNED_INT:
      std::cerr << "Entered cow::string must be unsigned integer: " << note << '\n';
      break;
    case ErrorCodes::INT_OUT_OF_RANGE:
      std::cerr << "Entered integer is too big: " << note << '\n';
      break;
    default:
      std::cerr << "Something going wrong\n";
      break;
  }
}

bool try_stoi(const char* a, int& res) {
  int i = 0;
  while (a[i] != '\0') {
    if (!isdigit(a[i])) {
      RaiseError(ErrorCodes::NOT_UNSIGNED_INT, a);
      return true;
    }
    ++i;
  }
  try {
    res = std::stoi(a);
  }
  catch(const std::exception& e) {
    RaiseError(ErrorCodes::INT_OUT_OF_RANGE, a);
    return true;
  }

  return false;
}

void read_to_stream(const cow::string& filepath, int count, char delim, bool forward_reading = true) {
  std::ifstream file(filepath.data(), std::ifstream::in);
  if (!file) {
    RaiseError(ErrorCodes::NO_SUCH_FILE, filepath.data());
  }

  auto file_pointer = std::ifstream::beg;
  file.seekg(-1, std::ifstream::end);
  auto size = file.tellg();

  char c;
  int offset = 0;
  if (!forward_reading) {
    int j = count;
    for (int i = 1; i <= size; ++i) {
      file.seekg(-i, std::ifstream::end);
      file.get(c);

      if (c == delim) {
        j--;
        if (j == 0) {
          offset = static_cast<int>(size) - i;
          break;
        }
      }
    }
  }

  file.seekg(offset, file_pointer);
  for (int i = 0; i <= static_cast<int>(size) - offset; ++i) {
    file.get(c);

    if (c == delim) {
      count--;
      if (count == 0) {
        break;
      }
    }

    std::cout << c;
  }
}

bool parse_delimiter(cow::string parameter, char& delimiter) {
  if (parameter.size() == 3 && parameter[0] == parameter[2] && parameter[0] == '\'') {
    delimiter = parameter[1];
    return true;
  }
  if (parameter.size() == 4 && parameter[0] == parameter[3] && parameter[0] == '\''
    && parameter[1] == '\\') {
    switch (parameter[2]) {
      case 'a':
        delimiter = '\a';
        break;
      case 'b':
        delimiter = '\b';
        break;
      case 't':
        delimiter = '\t';
        break;
      case 'n':
        delimiter = '\n';
        break;
      case 'v':
        delimiter = '\v';
        break;
      case 'f':
        delimiter = '\f';
        break;
      case 'r':
        delimiter = '\r';
        break;
      case '\\':
        delimiter = '\\';
        break;
      case '?':
        delimiter = '\?';
        break;
      case '\'':
        delimiter = '\'';
        break;
      default:
        return false;
    }
    return true;
  }

  return false;
}

bool parse_arguments(int argc, char* argv[], int& lines, bool& forward_reading, char& delimiter, cow::string& filepath) {
  for (int i = 1; i < argc; ++i) {
    cow::string curr_arg(argv[i]);
    if (curr_arg.size() > 1 && curr_arg[0] == '-') {
      if (curr_arg == "-t") {
        forward_reading = false;
      } else if (curr_arg == "-l") {
        ++i;
        if (try_stoi(argv[i], lines)) {
          return true;
        }
      } else if (curr_arg == "-d") {
        ++i;
        cow::string parameter(argv[i]);
        if (!parse_delimiter(parameter, delimiter)) {
          RaiseError(ErrorCodes::INCORRECT_DELIMITER, parameter.data());
          return true;
        }
      } else if (curr_arg == "--tail") {
        forward_reading = false;
      } else if (curr_arg.start_is("--lines=")) {
        if (try_stoi(curr_arg.substr(curr_arg.find('=') + 1).data(), lines)) {
          return true;
        }
      } else if (curr_arg.start_is("--delimiter=")) {
        cow::string parameter = curr_arg.substr(curr_arg.find('=') + 1);
        if (!parse_delimiter(parameter, delimiter)) {
          RaiseError(ErrorCodes::INCORRECT_DELIMITER, parameter.data());
          return true;
        }
      } else {
        RaiseError(ErrorCodes::NO_SUCH_ARGUMENT, curr_arg.data());
        return true;
      }
    } else if (filepath.size() == 0) {
      filepath = curr_arg;
    } else {
      RaiseError(ErrorCodes::ARGUMENT_NOT_SPECIFIED, curr_arg.data());
      return true;
    }
  }

  return false;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    RaiseError(ErrorCodes::FILE_NOT_SPECIFIED);
    return 1;
  }

  int lines = -1;
  bool forward_reading = true;
  char delimiter = '\n';
  cow::string filepath;

  if (parse_arguments(argc, argv, lines, forward_reading, delimiter, filepath)) {
    return 1;
  }

  read_to_stream(filepath, lines, delimiter, forward_reading);

  return 0;
}
