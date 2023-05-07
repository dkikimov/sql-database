//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_ERRORS_H_
#define LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_ERRORS_H_

enum ErrorTypes {
  SYNTAX_ERROR,
  UNKNOWN_DATA_TYPE,
  TABLE_NOT_FOUND
};

static const char* GetErrorTypeDescription(ErrorTypes error) {
  switch (error) {
    case SYNTAX_ERROR:
      return "Invalid command";
    case UNKNOWN_DATA_TYPE:
      return "Unknown data type";
    case TABLE_NOT_FOUND:
      return "Table not found";
  }
};

class SQLError: public std::exception {
 public:
  SQLError(SQLError const&) noexcept = default;

  SQLError& operator=(SQLError const&) noexcept = default;

  const char* what() const noexcept override { return GetErrorTypeDescription(error_); }
  explicit SQLError(ErrorTypes error) : error_(error) {}
  ~SQLError() override = default;
 private:
  ErrorTypes error_;
};
#endif //LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_ERRORS_H_