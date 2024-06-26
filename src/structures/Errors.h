//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_ERRORS_H_
#define LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_ERRORS_H_

enum ErrorTypes {
  SYNTAX_ERROR,
  SEMI_EXPECTED,
  UNKNOWN_DATA_TYPE,
  TABLE_NOT_FOUND,
  COLUMN_NOT_FOUND,
  MISMATCHED_DATA_TYPE,
  UNKNOWN_CONDITION,
  UNKNOWN_COMPARISON_OPERATOR,
  COLUMN_IS_NOT_NULL,
  FILE_TO_WRITE_CANT_OPEN,
  FILE_TO_READ_CANT_OPEN,
  TOO_MUCH_ARGUMENTS
};

static const char* GetErrorTypeDescription(ErrorTypes error) {
  switch (error) {
    case SYNTAX_ERROR:
      return "Invalid command";
    case UNKNOWN_DATA_TYPE:
      return "Unknown data type";
    case TABLE_NOT_FOUND:
      return "Table not found";
    case SEMI_EXPECTED:
      return "Semicolon expected";
    case COLUMN_NOT_FOUND:
      return "Column not found";
    case MISMATCHED_DATA_TYPE:
      return "Type of fields are mismatched";
    case UNKNOWN_CONDITION:
      return "Unknown condition";
    case UNKNOWN_COMPARISON_OPERATOR:
      return "Unknown comparison operator";
    case COLUMN_IS_NOT_NULL:
      return "You tried to add a value that has a nullable column, which is not null ";
    case FILE_TO_WRITE_CANT_OPEN:
      return "The database cannot be saved to the specified file";
    case FILE_TO_READ_CANT_OPEN:
      return "The database cannot be read from the specified file";
    case TOO_MUCH_ARGUMENTS:
      return "You have provided too much arguments";
  }
};

class SQLError: public std::exception {
 public:
  SQLError(SQLError const&) noexcept = default;

  SQLError& operator=(SQLError const&) noexcept = default;

  const char* what() const noexcept override { return GetErrorTypeDescription(error_); }

  ErrorTypes GetErrorType() const {
    return error_;
  }

  explicit SQLError(ErrorTypes error) : error_(error) {}
  ~SQLError() override = default;
 private:
  ErrorTypes error_;
};

class DBError: public std::exception {
 public:
  DBError(DBError const&) noexcept = default;

  DBError& operator=(DBError const&) noexcept = default;

  const char* what() const noexcept override { return GetErrorTypeDescription(error_); }

  ErrorTypes GetErrorType() const {
    return error_;
  }

  explicit DBError(ErrorTypes error) : error_(error) {}
  ~DBError() override = default;
 private:
  ErrorTypes error_;
};
#endif //LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_ERRORS_H_
