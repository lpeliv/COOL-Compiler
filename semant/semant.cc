

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}

ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /*------------------------------------------------*/
    /* Ulazak u scope */
    /*------------------------------------------------*/ 
    class_table.enterscope();

    /* Fill this in */
    install_basic_classes();
    
    /*------------------------------------------------*/
    /* Provjera klasa */
    /*------------------------------------------------*/

    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->fetchName();
        Symbol parent_name = current_class->fetchParent();

        /*------------------------------------------------*/
        /* Provjera pogrešno imenovanih klasa */
        /*------------------------------------------------*/
        if (class_name == Object) {
            semant_error(current_class) << "Class Object cannot be redefined.\n";
            continue;
        } 
        else if (class_name == SELF_TYPE) {
            semant_error(current_class) << "Class name cannot be SELF_TYPE.\n";
            continue;
        }
        /*------------------------------------------------*/
        /* Provjera redefiniranosti klasa */
        /*------------------------------------------------*/
        else if (class_name == Int || class_name == Bool || class_name == Str || class_name == IO) {
            semant_error(current_class) << "Redefinition of " << class_name << " is not allowed.\n";
            continue;
        }

        else  if( class_table.lookup(class_name) != NULL ){
            semant_error(current_class) << "Class " << class_name << " was previously defined." << endl;
            continue;
        } 

        /*------------------------------------------------*/
        /* Provjera valjanosti parent klasa */
        /*------------------------------------------------*/ 
        else if (parent_name == Bool || parent_name == Str || parent_name == Int || parent_name == SELF_TYPE) {
            semant_error(current_class) << "Cannot inherit from " << parent_name << ".\n";
            continue;
        }

        /*------------------------------------------------*/
        /* Provjera nasljeđivanja samog sebe */
        /*------------------------------------------------*/ 
        else if (class_name == parent_name) {
            semant_error(current_class) << "Class " << class_name << " cannot inherit from itself.\n";
            continue;
        }

        /*------------------------------------------------*/
        /* Provjera nasljeđivanja od nepostojeće klase */
        /*------------------------------------------------*/ 
        else if (class_table.lookup(parent_name) == NULL && parent_name != No_class) {
            semant_error(current_class) << "Class " << class_name << " inherits from an undefined class " << parent_name << ".\n";
            continue;
        }

       class_table.addid(class_name, &current_class);

    
    }

    /*------------------------------------------------*/
    /* Gradnja grafa nasljeđivanja */
    /*------------------------------------------------*/
    build_inheritance_graph(classes);
    
    /*------------------------------------------------*/
    /* Drugi prolaz */
    /*------------------------------------------------*/ 
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->fetchName();
        Symbol parent_name = current_class->fetchParent();
    }
    
    /*------------------------------------------------*/
    /* Provjera Main-a */
    /*------------------------------------------------*/
    int mainChecker = 0;
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        if (classes->nth(i)->fetchName() == Main) {
                mainChecker = 1;
                break;
            }
    }

    if (mainChecker == 0)
            semant_error() << "Class Main is not defined.\n";

    /*------------------------------------------------*/
    /* Popunjavanje mapa klasi */
    /*------------------------------------------------*/
    populate_class_map(classes);

    /*------------------------------------------------*/
    /* Izlazak iz scope-a */
    /*------------------------------------------------*/ 
    class_table.exitscope();
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
    // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    /*------------------------------------------------*/
    /* Dodavanje Object_class-a u tablicu simbola */
    /*------------------------------------------------*/
    class_table.addid(Object, &Object_class);
    
    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  
    
    /*------------------------------------------------*/
    /* Dodavanje IO_class-a u tablicu simbola */
    /*------------------------------------------------*/
    
    class_table.addid(IO, &IO_class);
    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);
    
    /*------------------------------------------------*/
    /* Dodavanje Int_class-a u tablicu simbola */
    /*------------------------------------------------*/ 
    class_table.addid(Int, &Int_class);
    
    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);
    
    /*------------------------------------------------*/
    /* Dodavanje Bool_class-a u tablicu simbola */
    /*------------------------------------------------*/ 
    class_table.addid(Bool, &Bool_class);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
    /*------------------------------------------------*/
    /* Dodavanje String_class-a u tablicu simbola */
    /*------------------------------------------------*/ 
    class_table.addid(Str, &Str_class);
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()
{                                                 
    semant_errors++;
    return error_stream;
} 



/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);
    
    /* some semantic analysis code may go here */

    if (classtable->errors()) {
	    cerr << "Compilation halted due to static semantic errors." << endl;
	    exit(1);
    }

    /*------------------------------------------------*/
    /* Ispis klasa i pokazivača njihovih definicija */
    /*------------------------------------------------*/ 
    classtable->dump_class_map();
    classtable->print_inheritance_graph();
}

void ClassTable::populate_class_map(Classes classes) {

    /*------------------------------------------------*/
    /* Dodavanje osnovnih klasa u mapu */
    /*------------------------------------------------*/ 
    class_map[Object] = *class_table.lookup(Object);
    class_map[IO] = *class_table.lookup(IO);
    class_map[Int] = *class_table.lookup(Int);
    class_map[Bool] = *class_table.lookup(Bool);
    class_map[Str] = *class_table.lookup(Str);

    /*------------------------------------------------*/
    /* Dodavanje korisnikovih klasa u mapu */
    /*------------------------------------------------*/ 
    
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->fetchName();

        /*------------------------------------------------*/
        /* Ukoliko nije prisutna, dodaj klasu u mapu */
        /*------------------------------------------------*/ 
        if (class_map.find(class_name) == class_map.end()) {
            class_map[class_name] = current_class;
        }
    }
}

/*------------------------------------------------*/
/* Ispis imena i definicije klase */
/*------------------------------------------------*/ 
void ClassTable::dump_class_map() {
    for (const auto& pair : class_map) {
        cerr << "Class Name: " << pair.first << ", Class Definition: " << pair.second << endl;
    }
}


/*------------------------------------------------*/
/* Izgradnja grafa nasljeđivanja */
/*------------------------------------------------*/ 
void ClassTable::build_inheritance_graph(Classes classes) {
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->fetchName();
        Symbol parent_name = current_class->fetchParent();
        
        if (parent_name != No_class) {
            inheritance_graph[parent_name].push_back(class_name);
        }
    }
}

/*------------------------------------------------*/
/* Ispis imena i nasljeđivanja klase */
/*------------------------------------------------*/ 
void ClassTable::print_inheritance_graph() {
    for (const auto& pair : inheritance_graph) {
        cerr << "Class " << pair.first << " is inherited by:\n";
        for (const Symbol& subclass : pair.second) {
            cerr << "  " << subclass << "\n";
        }
    }
}