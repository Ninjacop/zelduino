#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>

/// ------------------------------------------
/// TOKEN ENUM
///   stores all of the possible token types
/// ------------------------------------------
enum Token {
  tok_eof = -1,

  tok_instruction = -2,
  tok_duration = -3,

  tok_loop = -4,
  tok_end = -5
};

/// ------------------------------------------------
/// HELPER FUNCTIONS
///   strupper() -> converts a string to UPPERCASE
/// ------------------------------------------------
std::string strupper(std::string str) {
  for (int i = 0; i < str.size(); i++) {
      str[i] = toupper(str[i]);
  }

  return str;
}

/// ------------------------------------------------------
/// LEXER
///   id_str  -> store non numerical tokens here
///   num_val -> store numerical tokens here
///   filestr -> store an input file into this string
///   instructions -> output file
///   gettok() -> get a singular token from input string
/// ------------------------------------------------------
static std::string id_str;
static int num_val;
std::string filestr;
FILE *instructions = fopen("controller/instructions.h", "w");

// Get a singular token from a string, then return the type of
// token that was read
int gettok(std::string instr) {
  // read token character by character
  static char last_char;
  // return the token type in `rettok', once exited from loop
  // if nothing was parsed, return the last character in the string
  int rettok = instr[instr.size()];

  // iterate through each character of the string
  for (int i = 0; i < instr.size(); i++) {
    // update the last character read
    last_char = instr[i];

    // if last character was whitespace, continue over it
    while (isspace(last_char)) {
      last_char = instr[++i];
    }

    // if the last character was not whitespace or a number,
    // start lexing an instruction
    if (isalpha(last_char)) {
      // Set id_str to the instruction word
      id_str = last_char;
      last_char = instr[++i];

      while (isalnum(last_char)) {
        id_str += last_char;
        last_char = instr[++i];
      }

      // cases for reserved tokens
      // return loop token and delete read tokens
      if (id_str == "loop") {
        filestr = instr.substr(i);
        rettok = tok_loop;
        break;
      }

      // return end toke and delete read tokens
      if (id_str == "end") {
        filestr = instr.substr(i);
        rettok = tok_end;
        break;
      }

      // if not a reserved token, return instruction token
      // and delete read tokens
      filestr = instr.substr(i);
      rettok = tok_instruction;
      break;
    }

    // if the last character was a number,
    // start lexing a duration
    if (isdigit(last_char)) {
      std::string num_str;

      // collect all digits in num_str
      do {
        num_str += last_char;
        last_char = instr[++i];
      } while (isdigit(last_char));

      // convert num_str to number, store it in num_val,
      // then delete read tokens and return duration token
      num_val = strtod(num_str.c_str(), 0);
      filestr = instr.substr(i);
      rettok = tok_duration;
      break;
    }

    // comment until end of line
    if (last_char == '%') {
      do
        last_char = instr[++i];
      while (last_char != '\0' && last_char != '\r' && last_char != '\n');
    }

    // comment from [ to ]
    if (last_char == '[') {
      do
        last_char = instr[++i];
      while (last_char != ']');
    }

    // return EOF token if the last character read was and EOF
    if (last_char == EOF) {
      filestr = instr.substr(i);
      rettok = tok_eof;
      break;
    }

    // delete read tokens, in case the last character was none of the above
    filestr = instr.substr(i);
  }

  // if nothing was lexed, return 0
  if (rettok == instr[instr.size()]) {
    return 0;
  }

  // otherwise, return the token type
  return rettok;
}

/// ----------------------------------------------
/// AST
///   AST -> base AST class
///   NumberAST -> AST for numerical literals
///   InstructionAST -> AST for instructions/words
///   LoopAST -> AST for `loop'
/// ----------------------------------------------

// Base AST for all expression nodes
class AST {
  public:
    virtual ~AST() {}
};

// AST for numerical literals
class NumberAST : public AST {
  double val;

  public:
    NumberAST(double val) : val(val) {}
};

// AST for referencing a variable
class InstructionAST : public AST {
  std::string name;

  public:
    InstructionAST(const std::string &name) : name(name) {}
};

// AST for looping
class LoopAST : public AST {
  int times;
  std::vector<std::string> body;

  public:
    LoopAST(int times, std::vector<std::string> body)
      : times(times), body(body) {}
};

/// ---------------------------------------------------------
/// PARSER
///   currtok -> current token type being parsed
///   duracount -> amount of durations in a file
///   instrcount -> amount of instructions in a file
///   nexttoken() -> wrapper for gettok()
///   LogError() -> raise an error, if encountered
///   ParseNumber() -> parse a number
///   ParseInstruction() -> parse an instruction
///   ParseLoop() -> parse a loop function
///   ParsePrimary() -> wrapper to parse the above functions
/// ---------------------------------------------------------

// forward declaration for loop statement
static std::unique_ptr<AST> ParsePrimary();

static int currtok;
static int duracount = 0;
static int instrcount = 0;
static int nexttoken() {
  return currtok = gettok(filestr);
}

// LogError* -- helper functions for error handling
std::unique_ptr<AST> LogError(const char *str) {
  fprintf(stderr, "LogError: %s\n" ,str);
  return nullptr;
}

// Parse numerical literals
std::unique_ptr<AST> ParseNumber() {
  auto result = std::make_unique<NumberAST>(num_val);

  // write to instructions file, get next token and 
  // increase amount of duration tokens
  fprintf(instructions, "%d},\n", num_val);
  nexttoken();
  duracount++;
  return std::move(result);
}

// Parse instruction expression
std::unique_ptr<AST> ParseInstruction() {
  std::string id_name = id_str;
  std::string uppername = strupper(id_str);

  // write to instruction file, get next token and 
  // increase the amount of instruction tokens
  fprintf(instructions, "\t{%s, ", uppername.c_str());
  nexttoken();
  instrcount++;
  return std::make_unique<InstructionAST>(id_name);
}

// Parse loop expression
std::unique_ptr<AST> ParseLoop() {
  // eat `loop' token and set amount of times
  // it will execute
  nexttoken();
  int times = num_val;

  // collect body of loop in args, concatenate them in strargs,
  // save strargs so they can be used again, then save filestr
  // so the file can be restored after executing loop statement
  std::vector<std::string> args;
  std::string strargs, save_strargs;
  std::string save_filestr = filestr;

  // collect body of loop statement 
  while (nexttoken() != tok_end) {
    // if there is no end to the loop, raise an error
    if (currtok == 0 || currtok == tok_eof) {
      LogError("Unable to exit loop -- missing `end'\n");
      remove("controller/instructions.h");
      return nullptr;
    }

    // push back the current argument into args, 
    // and convert them to a string if necessary
    switch (currtok) {
      case tok_instruction:
        args.push_back(id_str);
        break;
      case tok_duration:
        args.push_back(std::to_string(num_val));
      default:
        break;
    }
  }

  // concatenate all the arguments into a single string
  strargs = args.at(0);
  for (int i = 1; i < args.size(); i++) {
    strargs += " ";
    strargs += args.at(i);
  }

  // set the filestr to the instructions inside the loop.
  // then save it so it can be used again
  filestr = strargs;
  save_strargs = strargs;

  // parse the loop body
  for (int i = 0; i < times; i++) {
    nexttoken();
    while (currtok < 0) {
      ParsePrimary();
    }

    // restore the filestr to the loop body,
    // so it can be parsed again
    filestr = strargs;
  }

  // restore filestr
  filestr = save_filestr;

  // delete all the tokens until it reaches the end of the loop,
  // which has now been processed
  while (currtok != tok_end) {
    // if it encounters another loop or EOF, raise an error
    // otherwise, continue
    if (currtok == tok_loop || currtok == tok_eof) {
      LogError("Unable to exit loop -- missing `end'\n");
      remove("controller/instructions.h");
      return nullptr;
    } else nexttoken();
  }

  // eat `end' and return
  nexttoken();
  return std::make_unique<LoopAST>(times, args);
}

// Parse primary expressions
static std::unique_ptr<AST> ParsePrimary() {
  // Parse the current token, according to it's type
  switch (currtok) {
    default:
      // if the token is not `end', then raise an error
      // `end' doesn't need to be parsed, so it can be skipped,
      // as it is only ever used in a loop, and it covers that there
      if (currtok != tok_end)
        LogError("Unknown token when expecting an expression.");
        remove("controller/instructions.h");
        return nullptr;
      case tok_instruction:
        return ParseInstruction();
      case tok_duration:
        return ParseNumber();
      case tok_loop:
        return ParseLoop();
    }
}

/// ---------------------
/// main loop driver
/// ---------------------
static void mainloop() {
  // While the current token is a valid token, parse input.
  // Also, if parsing raises an error, exit loop
  while (currtok < 0) {
    if (!ParsePrimary()) {
      return;
    }
  }

  // If the instruction and duration count is uneven, raise 
  // an error, because for every instruction there's a duration.
  // Times number in loop statements are omitted
  if (instrcount != duracount) {
    LogError("Uneven amount of instructions and durations.");
    printf("%di, %dd\n", instrcount, duracount);
    remove("controller/instructions.h");
  }
}

/// ---------------------
/// MAIN
/// ---------------------
int main(int argc, char *argv[]) {
  // Read the input file into a string
  // Check if the argument exists and the output file is ready
  if (argv[1] || instructions != NULL) { 
    std::ifstream infile(argv[1]);
    std::string str((std::istreambuf_iterator<char>(infile)),
                   std::istreambuf_iterator<char>());
    filestr = str;
  } else {
    LogError("No inputted file, or outfile was not ready.");
    LogError("Usage: zelduino [input file]");
    remove("controller/instructions.h");
    return 1;
  }

  // Write file setup into instructions header
  fprintf(instructions, "#include <stdint.h>\n\ntypedef enum {\n\tUP,");
  fprintf(instructions, "\n\tDOWN,\n\tLEFT,\n\tRIGHT,\n\tX,\n\tY,\n");
  fprintf(instructions, "\tA,\n\tB,\n\tL,\n\tR,\n\tZL,\n\tZR,\n\t");
  fprintf(instructions, "LCLICK,\n\tRCLICK,\n\tMINUS,\n\tPLUS,\n\tHOME");
  fprintf(instructions, ",\n\tCAPTURE,\n\tTHROW,\n\tNOTHING,\n\t");
  fprintf(instructions, "TRIGGERS\n} Buttons_t;\n\ntypedef struct {\n");
  fprintf(instructions, "\tButtons_t button;\n\tuint16_t duration;\n}");
  fprintf(instructions, " command;\n\nstatic const command step[] = {\n");

  // Get a token from a file, and start to parse it
  nexttoken();
  mainloop();

  // Remove trailing comma at the end of all the instruction pairs
  long int pos = ftell(instructions);
  fseek(instructions, pos - 2, SEEK_SET);

  // Finish with the instructions header
  fprintf(instructions, "\n};\n");
  fclose(instructions);

  return 0;
}