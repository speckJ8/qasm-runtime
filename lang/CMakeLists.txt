add_library(Context context.hpp)
set_target_properties(Context PROPERTIES LINKER_LANGUAGE CXX)
add_library(Error error.hpp)
set_target_properties(Error PROPERTIES LINKER_LANGUAGE CXX)
add_library(Expression expression.cc)
add_library(Input input.cc)
add_library(Lexer lexer.cc)
add_library(List list.hpp)
add_library(Parser parser.cc)
add_library(Program program.hpp)
set_target_properties(Program PROPERTIES LINKER_LANGUAGE CXX)
add_library(Sema sema.cc)
add_library(Statement statement.cc)
add_library(SymbolTable symbol_table.cc)

target_link_libraries(Error PUBLIC Context)
target_link_libraries(Error PUBLIC Input)
target_link_libraries(Expression PUBLIC SymbolTable)
target_link_libraries(Input PUBLIC Context)
target_link_libraries(Lexer PUBLIC Input)
target_link_libraries(List PUBLIC Context Expression)
target_link_libraries(Parser PUBLIC Error
                                    Expression
                                    List
                                    Input
                                    Lexer
                                    Program
                                    Statement)
target_link_libraries(Program PUBLIC Statement)
target_link_libraries(Sema PUBLIC Error
                                  Expression
                                  List
                                  Input
                                  Lexer
                                  Program
                                  Statement
                                  SymbolTable)
target_link_libraries(Statement PUBLIC Context
                                       Error
                                       Expression
                                       List
                                       SymbolTable)

add_library(Lang INTERFACE)
target_link_libraries(Lang INTERFACE Error)
target_link_libraries(Lang INTERFACE Input)
target_link_libraries(Lang INTERFACE Parser)
target_link_libraries(Lang INTERFACE Program)
target_link_libraries(Lang INTERFACE Sema)
target_link_libraries(Lang INTERFACE SymbolTable)
target_link_libraries(Lang INTERFACE Runtime)
