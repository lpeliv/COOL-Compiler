#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>
#include <map>
#include <vector>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
  int semant_errors;
  void install_basic_classes();
  ostream& error_stream;

  /*------------------------------------------------*/
  /* Tablica simbola */
  /*------------------------------------------------*/
  SymbolTable<Symbol, Class_> class_table;

  /*------------------------------------------------*/
  /* Mapa klasi */
  /*------------------------------------------------*/
  std::map<Symbol, Class_> class_map;

  /*------------------------------------------------*/
  /* Graf nasljeđivanja */
  /*------------------------------------------------*/
  std::map<Symbol, std::vector<Symbol>> inheritance_graph;

public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);

  /*------------------------------------------------*/
  /* Metoda pristupa tablici simbola */
  /*------------------------------------------------*/
  SymbolTable<Symbol, Class_>& get_class_table() { return class_table; }
  
  /*------------------------------------------------*/
  /* Metoda popunjavanja i odbacivanja mapa klasi */
  /*------------------------------------------------*/
  void populate_class_map(Classes classes);
  void dump_class_map();

  /*------------------------------------------------*/
  /* Metoda za gradnju i ispis grafa nasljeđivanja */
  /*------------------------------------------------*/
  void build_inheritance_graph(Classes classes);
  void print_inheritance_graph();
};

#endif

