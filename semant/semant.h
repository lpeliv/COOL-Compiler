#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>
#include <map>
#include <set>
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
  SymbolTable<Symbol, Class_> symbol_table;

  /*------------------------------------------------*/
  /* Mapa klasi */
  /*------------------------------------------------*/
  std::map<Symbol, Class_> class_map;

  /*------------------------------------------------*/
  /* Graf nasljeđivanja */
  /*------------------------------------------------*/
  std::map<Symbol, std::set<Symbol>> inheritance_graph;

  void collect_ancestors(Symbol class_name, std::set<Symbol>& ancestors);
  bool detect_cycle(Symbol class_name, std::set<Symbol>& visited, std::set<Symbol>& rec_stack);

public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);

  /*------------------------------------------------*/
  /* Metoda pristupa tablici simbola */
  /*------------------------------------------------*/
  SymbolTable<Symbol, Class_>& get_class_table() { return symbol_table; }
  
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

/*------------------------------------------------*/
/* Definicija walk_down_add metode */
/*------------------------------------------------*/
void class__class::walk_down_add(ClassTable *ct) {
  Symbol class_name = this->fetchName();
  cerr << "Test " << class_name << "\n";
}

#endif

