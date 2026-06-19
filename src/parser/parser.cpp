#include <iostream>
#include <memory>
#include <sched.h>
#include <exception>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <assert.h>

#include <vector>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

#include "parser.h"
#include "ast.h"
#include "tao/pegtl/ascii.hpp"
#include "tao/pegtl/internal/pegtl_string.hpp"


namespace pegtl = TAO_PEGTL_NAMESPACE;

using namespace pegtl;

namespace parser {

  struct str_arrow : TAO_PEGTL_STRING( "<-" ) {};
  struct str_goto : TAO_PEGTL_STRING( "goto" ) {};
  struct str_return : TAO_PEGTL_STRING( "return" ) {};
  struct str_call : TAO_PEGTL_STRING( "call" ) {};

  struct comment_rule: 
    pegtl::disable< 
      TAO_PEGTL_STRING( "//" ), 
      pegtl::until< pegtl::eolf > 
    > {};

  struct spaces_rule:
    pegtl::star< 
      pegtl::sor<
        pegtl::one< ' ' >,
        pegtl::one< '\t'>
      >
    > { };

  struct seps_rule : 
    pegtl::star<
      pegtl::seq<
        spaces_rule,
        pegtl::eol
      >
    > { };
  struct seps_with_comments_rule : 
    pegtl::star< 
      pegtl::seq<
        spaces_rule,
        pegtl::sor<
          pegtl::eol,
          comment_rule
        >
      >
    > { };

  struct name_rule:
    pegtl::seq<
      pegtl::plus< 
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >
        >
      >,
      pegtl::star<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >,
          pegtl::digit
        >
      >
    > {};

  struct names_rule:
    pegtl::seq<
      spaces_rule,
      name_rule,
      pegtl::star<
        pegtl::seq<
          spaces_rule,
          pegtl::one<','>,
          spaces_rule,
          name_rule
        >
      >
    > {};

  struct number_rule:
    pegtl::seq<
      pegtl::opt<
        pegtl::sor<
          pegtl::one< '-' >,
          pegtl::one< '+' >
        >
      >,
      pegtl::plus< 
        pegtl::digit
      >
    >{};

  struct label_rule:
    pegtl::seq<
      pegtl::one<':'>,
      name_rule
    > {};

  struct array_type_rule:
    pegtl::seq<
      TAO_PEGTL_STRING( "int64" ),
      pegtl::plus<
        TAO_PEGTL_STRING( "[]" )
      >
    > {};

  struct type_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<array_type_rule>, array_type_rule>,
      TAO_PEGTL_STRING( "int64" )
    > {}; 
  
  struct return_type_rule:
    pegtl::sor<
      type_rule,
      TAO_PEGTL_STRING( "void" )
    > {};

  struct par_rule:
    pegtl::seq<
      type_rule,
      spaces_rule,
      name_rule
    > {};

  struct pars_rule:
    pegtl::sor<
      pegtl::seq<
        spaces_rule,
        par_rule,
        spaces_rule,
        pegtl::star<
          pegtl::seq<
            spaces_rule,
            pegtl::one<','>,
            spaces_rule,
            par_rule,
            spaces_rule
          >
        >
      >,
      pegtl::seq<
        spaces_rule,
        par_rule,
        spaces_rule
      >,
      spaces_rule
    > {};


  struct cmp_rule:
    pegtl::sor<
      TAO_PEGTL_STRING( "<=" ),
      TAO_PEGTL_STRING( "<" ),
      TAO_PEGTL_STRING( "=" ),
      TAO_PEGTL_STRING( ">=" ),
      TAO_PEGTL_STRING( ">" )
    > {};

  struct op_rule:
    pegtl::sor<
      TAO_PEGTL_STRING( "+" ),
      TAO_PEGTL_STRING( "-" ),
      TAO_PEGTL_STRING( "*" ),
      TAO_PEGTL_STRING( "&" ),
      TAO_PEGTL_STRING( "<<" ),
      TAO_PEGTL_STRING( ">>" ),
      cmp_rule
      > {};

  struct name_or_num_rule:
    pegtl::sor<
      name_rule,
      number_rule
    > {};

  struct args_rule:
    pegtl::sor<
      pegtl::seq<
        spaces_rule,
        name_or_num_rule,
        spaces_rule,
        pegtl::star<
          pegtl::seq<
            spaces_rule,
            pegtl::one<','>,
            spaces_rule,
            name_or_num_rule,
            spaces_rule
          >
        >
      >,
      pegtl::seq<
        spaces_rule,
        name_or_num_rule,
        spaces_rule
      >,
      spaces_rule
    > {};

  struct Instruction_declare_rule:
    pegtl::seq<
      type_rule,
      spaces_rule,
      names_rule
    > {};

  struct Instruction_assign_rule:
    pegtl::seq<
      name_rule,
      spaces_rule,
      str_arrow,
      spaces_rule,
      name_or_num_rule
    > {};

  struct Instruction_assign_op_rule:
    pegtl::seq<
      name_rule,
      spaces_rule,
      str_arrow,
      spaces_rule,
      name_or_num_rule,
      spaces_rule,
      op_rule,
      spaces_rule,
      name_or_num_rule
    > {};

  struct Instruction_label_rule:
    label_rule {};

  struct Instruction_goto_rule:
    pegtl::seq<
      str_goto,
      spaces_rule,
      label_rule
    > {};

  struct Instruction_return_void_rule:
    TAO_PEGTL_STRING( "return" ) {};
  
  struct Instruction_return_rule:
    pegtl::seq<
      TAO_PEGTL_STRING( "return" ),
      spaces_rule,
      name_or_num_rule
    > {};

  struct array_access_rule:
    pegtl::plus<
      pegtl::seq<
        pegtl::one<'['>,
        name_or_num_rule,
        pegtl::one<']'>
      >
    > {};

  struct Instruction_get_array_rule:
    pegtl::seq<
      name_rule,
      spaces_rule,
      str_arrow,
      spaces_rule,
      name_rule,
      array_access_rule
    > {};

  struct Instruction_set_array_rule:
    pegtl::seq<
      name_rule,
      array_access_rule,
      spaces_rule,
      str_arrow,
      spaces_rule,
      name_or_num_rule
    > {};

  struct Instruction_length_dim_rule:
    pegtl::seq<
      name_rule,
      spaces_rule,
      str_arrow,
      spaces_rule,
      TAO_PEGTL_STRING( "length" ),
      spaces_rule,
      name_rule,
      spaces_rule,
      name_or_num_rule
    > {};

    struct Instruction_call_rule:
      pegtl::seq<
        name_rule,
        spaces_rule,
        pegtl::one<'('>,
        spaces_rule,
        args_rule,
        spaces_rule,
        pegtl::one<')'>
      > {};

  struct Instruction_assign_call_rule:
    pegtl::seq<
      name_rule,
      spaces_rule,
      str_arrow,
      spaces_rule,
      name_rule,
      spaces_rule,
      pegtl::one<'('>,
      spaces_rule,
      args_rule,
      spaces_rule,
      pegtl::one<')'>
    > {};

  struct Instruction_create_array_rule:
    pegtl::seq<
      name_rule,
      spaces_rule,
      str_arrow,
      spaces_rule,
      TAO_PEGTL_STRING( "new Array" ),
      spaces_rule,
      pegtl::one<'('>,
      spaces_rule,
      args_rule,
      spaces_rule,
      pegtl::one<')'>
    > {};

  struct cond_rule:
    pegtl::seq<
      name_or_num_rule,
      spaces_rule,
      cmp_rule,
      spaces_rule,
      name_or_num_rule
    > {};

  struct Instruction_while_rule:
    pegtl::seq<
      TAO_PEGTL_STRING( "while" ),
      spaces_rule,
      pegtl::one<'('>,
      spaces_rule,
      cond_rule,
      spaces_rule,
      pegtl::one<')'>,
      spaces_rule,
      label_rule,
      spaces_rule,
      label_rule
    > {};

  struct Instruction_if_rule:
    pegtl::seq<
      TAO_PEGTL_STRING( "if" ),
      spaces_rule,
      pegtl::one<'('>,
      spaces_rule,
      cond_rule,
      spaces_rule,
      pegtl::one<')'>,
      spaces_rule,
      label_rule,
      spaces_rule,
      label_rule
    > {};


  struct Instruction_continue_rule:
    TAO_PEGTL_STRING( "continue" ) {};

  struct Instruction_break_rule:
    TAO_PEGTL_STRING( "break" ) {};

  struct scope_entry_rule:
    pegtl::one<'{'> {};

  struct scope_exit_rule:
    pegtl::one<'}'> {};

  struct Instructions_rule;
  struct scope_rule:
    pegtl::seq<
      spaces_rule,
      seps_with_comments_rule,
      spaces_rule,
      scope_entry_rule,
      Instructions_rule,
      seps_with_comments_rule,
      spaces_rule,
      scope_exit_rule
    > {};

  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_while_rule>, Instruction_while_rule>,
      pegtl::seq< pegtl::at<Instruction_if_rule>, Instruction_if_rule>,
      pegtl::seq< pegtl::at<Instruction_continue_rule>, Instruction_continue_rule>,
      pegtl::seq< pegtl::at<Instruction_break_rule>, Instruction_break_rule>,
      pegtl::seq< pegtl::at<Instruction_assign_op_rule>, Instruction_assign_op_rule>,
      pegtl::seq< pegtl::at<Instruction_get_array_rule>, Instruction_get_array_rule>,
      pegtl::seq< pegtl::at<Instruction_set_array_rule>, Instruction_set_array_rule>,
      pegtl::seq< pegtl::at<Instruction_length_dim_rule>, Instruction_length_dim_rule>,
      pegtl::seq< pegtl::at<Instruction_create_array_rule>, Instruction_create_array_rule>,
      pegtl::seq< pegtl::at<Instruction_call_rule>, Instruction_call_rule>,
      pegtl::seq< pegtl::at<Instruction_assign_call_rule>, Instruction_assign_call_rule>,
      pegtl::seq< pegtl::at<Instruction_assign_rule>, Instruction_assign_rule>,
      pegtl::seq< pegtl::at<Instruction_declare_rule>, Instruction_declare_rule>,
      pegtl::seq< pegtl::at<Instruction_return_rule>, Instruction_return_rule>,
      pegtl::seq< pegtl::at<Instruction_return_void_rule>, Instruction_return_void_rule>,
      pegtl::seq< pegtl::at<Instruction_goto_rule>, Instruction_goto_rule>,
      pegtl::seq< pegtl::at<Instruction_label_rule>, Instruction_label_rule>
    > {};

  struct Instructions_rule:
    pegtl::star<
      pegtl::sor<
        pegtl::seq< pegtl::at<scope_rule>, scope_rule>,
        pegtl::seq<
          seps_with_comments_rule,
          pegtl::bol,
          spaces_rule,
          Instruction_rule,
          seps_rule
        >
      >
    > { };

  struct function_rule :
    pegtl::seq<
      seps_with_comments_rule,

      pegtl::seq<spaces_rule, return_type_rule>,
      seps_with_comments_rule,
      
      pegtl::seq<spaces_rule, name_rule>,
      seps_with_comments_rule,

      pegtl::seq<spaces_rule, pegtl::one<'('>>,
      seps_with_comments_rule,

      pars_rule,

      seps_with_comments_rule,
      pegtl::seq<spaces_rule, pegtl::one<')'>>,

      scope_rule
    > {};

  struct entry_point_rule:
    pegtl::plus<
      seps_with_comments_rule,
      function_rule,
      seps_with_comments_rule
    > {};

  struct grammar: 
    pegtl::must< 
      entry_point_rule
    > {};



  std::vector<std::unique_ptr<ASTNode>> parsed_items;

  void print_parsed_items() {
    for (const auto &item: parsed_items) {
      std::cout << item->to_string() << "\n";
    }
  }

  std::unique_ptr<ASTNode> pop() {
    auto back = std::move(parsed_items.back());
    parsed_items.pop_back();
    return back;
  }

  template<typename... Args>
  std::vector<std::unique_ptr<ASTNode>> create_children(Args&&... args) {
    std::vector<std::unique_ptr<ASTNode>> v;
    v.reserve(sizeof...(args));
    (v.push_back(std::forward<Args>(args)), ...);
    return v;
  }

  template< typename Rule >
    struct action : pegtl::nothing< Rule > {};

  template<> struct action < number_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p) {
      parsed_items.push_back(std::make_unique<Number>(std::stoll(in.string())));
    }
  };

  template<> struct action < name_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      parsed_items.push_back(std::make_unique<Identifier>(in.string()));
    }
  };

  template<> struct action < label_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      parsed_items.pop_back(); // remove existing Name for this label
      parsed_items.push_back(std::make_unique<Identifier>(in.string()));
    }
  };

  template<> struct action < cmp_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      if (in.string() == "<") {
        parsed_items.push_back(std::make_unique<Op>(EOp::LT));
      } else if (in.string() == "<=") {
        parsed_items.push_back(std::make_unique<Op>(EOp::LTEQ));
      } else if (in.string() == "=") {
        parsed_items.push_back(std::make_unique<Op>(EOp::EQ));
      } else if (in.string() == ">") {
        parsed_items.push_back(std::make_unique<Op>(EOp::GT));
      } else if (in.string() == ">=") {
        parsed_items.push_back(std::make_unique<Op>(EOp::GTEQ));
      }
    }
  };

  template<> struct action < op_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      if (in.string() == "+") {
        parsed_items.push_back(std::make_unique<Op>(EOp::ADD));
      } else if (in.string() == "-") {
        parsed_items.push_back(std::make_unique<Op>(EOp::SUB));
      } else if (in.string() == "*") {
        parsed_items.push_back(std::make_unique<Op>(EOp::MUL));
      } else if (in.string() == "&") {
        parsed_items.push_back(std::make_unique<Op>(EOp::AND));
      } else if (in.string() == "<<") {
        parsed_items.push_back(std::make_unique<Op>(EOp::L_SHIFT));
      } else if (in.string() == ">>") {
        parsed_items.push_back(std::make_unique<Op>(EOp::R_SHIFT));
      }
    }
  };

  template<> struct action < array_type_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto in_str = in.string();
      uint64_t dims = std::count(in_str.begin(), in_str.end(), '[');
      parsed_items.push_back(std::make_unique<ArrayType>(dims));
    }
  };

  template<> struct action < type_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      if (in.string() == "int64") {
        parsed_items.push_back(std::make_unique<Type>(EType::INT64));
      }
    }
  };

  template<> struct action < return_type_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){

      auto in_str = in.string();      
      if (in_str == "void") {
        parsed_items.push_back(std::make_unique<Type>(EType::VOID));
      }
    }
  };

  template<> struct action < par_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){

      auto var = pop();
      auto type = pop();

      parsed_items.emplace_back(std::make_unique<instr::Decl>(
        create_children(std::move(type), std::move(var))));
    }
  };

  template<> struct action < pars_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      bool is_empty = std::ranges::all_of(in.string(), ::isspace);
      if (is_empty) {
        parsed_items.push_back(std::make_unique<Params>());
      } else {
        size_t n_pars = std::ranges::count(in.string(), ',') + 1;
        std::vector<std::unique_ptr<ASTNode>> pars;
        for (size_t i = 0; i < n_pars; i++) {
          auto par = pop();
          pars.push_back(std::move(par));
        }
        std::reverse(pars.begin(), pars.end());
        parsed_items.emplace_back(std::make_unique<Params>(std::move(pars)));
      }
    }
  };

  template<> struct action < args_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      bool is_empty = std::ranges::all_of(in.string(), ::isspace);
      if (is_empty) {
        parsed_items.push_back(std::make_unique<Args>());
      } else {
        size_t n_args = std::ranges::count(in.string(), ',') + 1;
        std::vector<std::unique_ptr<ASTNode>> args;
        for (size_t i = 0; i < n_args; i++) {
          auto t = pop();
          args.push_back(std::move(t));
        }
        std::reverse(args.begin(), args.end());
        parsed_items.push_back(std::make_unique<Args>(std::move(args)));
      }
    }
  };

  template<> struct action < cond_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto t2 = pop();
      auto op = pop();
      auto t1 = pop();
      parsed_items.push_back(std::make_unique<Cond>(create_children(std::move(t1), std::move(op), std::move(t2))));
    }
  }; 


  template<> struct action < scope_entry_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      std::cout << "scope entry\n";
      print_parsed_items();
      parsed_items.push_back(std::make_unique<Scope>());
    }
  }; 

  template<> struct action < scope_exit_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto scope = pop();

      // Otherwise, we are at the end of a function (handled in function_rule)
      if (!parsed_items.empty() && dynamic_cast<Scope*>(parsed_items.back().get())) {
        parsed_items.back()->add_child(std::move(scope));
      } else {
        parsed_items.push_back(std::move(scope));
      }
    }
  };

  template<> struct action < Instruction_declare_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto var = pop();
      auto type = pop();
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Decl>(create_children(std::move(type), std::move(var))));
    }
  }; 

  template<> struct action < Instruction_assign_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto t = pop();
      auto var = pop();
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Assign>(create_children(std::move(var), std::move(t))));
    }
  }; 

  template<> struct action < Instruction_assign_op_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){

      auto t2 = pop();
      auto op = pop();
      auto t1 = pop();
      auto var = pop();
      auto &scope = parsed_items.back();

      scope->add_child(std::make_unique<instr::AssignOp>(create_children(std::move(var), std::move(t1), std::move(op), std::move(t2))));
    }
  }; 

  template<> struct action < Instruction_label_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){

      auto l = std::make_unique<Identifier>(in.string());
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Label>(create_children(std::move(l))));
    }
  }; 

  template<> struct action < Instruction_if_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){

      auto l2 = pop();
      auto l1 = pop();
      auto cond = pop();
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::If>(create_children(std::move(cond), std::move(l1), std::move(l2))));
    }
  }; 

  template<> struct action < Instruction_while_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){

      auto l2 = pop();
      auto l1 = pop();
      auto cond = pop();
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::While>(create_children(std::move(cond), std::move(l1), std::move(l2))));
    }
  }; 

  template<> struct action < Instruction_continue_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Continue>());
    }
  }; 

  template<> struct action < Instruction_break_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Break>());
    }
  }; 

  template<> struct action < Instruction_get_array_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto in_str = in.string();
      std::vector<std::unique_ptr<ASTNode>> idxs;
      size_t dims = std::count(in_str.begin(), in_str.end(), '[');
      for (int i = 0; i < dims; i++) {
        idxs.push_back(pop());
      }
      std::reverse(idxs.begin(), idxs.end());
      auto arr_name = pop();
      auto var = pop();

      auto children = create_children(std::move(var), std::move(arr_name));
      for (auto& idx : idxs) {
        children.push_back(std::move(idx));
      }

      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::ArrGet>(std::move(children)));
    }
  }; 

  template<> struct action < Instruction_set_array_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto in_str = in.string();

      auto val = pop();

      std::vector<std::unique_ptr<ASTNode>> idxs;
      size_t dims = std::count(in_str.begin(), in_str.end(), '[');
      for (int i = 0; i < dims; i++) {
        idxs.push_back(pop());
      }
      std::reverse(idxs.begin(), idxs.end());
      auto arr_name = pop();

      auto children = create_children(std::move(arr_name));
      for (auto& idx : idxs) {
        children.push_back(std::move(idx));
      }
      children.push_back(std::move(val));

      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::ArrSet>(std::move(children)));
    }
  }; 

  template<> struct action < Instruction_length_dim_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto dim = pop();
      auto arr_name = pop();
      auto var = pop();
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Length>(create_children(std::move(var), std::move(arr_name), std::move(dim))));
    }
  }; 

  template<> struct action < Instruction_call_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto args = pop();
      auto func_name = pop();

      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Call>(create_children(std::move(func_name), std::move(args))));
    }
  }; 

  template<> struct action < Instruction_assign_call_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto args = pop();
      auto func_name = pop();
      auto var = pop();

      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Call>(create_children(std::move(var), std::move(func_name), std::move(args))));
    }
  };


  template<> struct action < Instruction_create_array_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto args = pop();
      auto var = pop();

      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::CreateArray>(create_children(std::move(var), std::move(args))));
    }
  };

  template<> struct action < Instruction_return_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto var = pop();
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Return>(create_children(std::move(var))));
    }
  };

  template<> struct action < Instruction_return_void_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Return>());
    }
  };
  
  template<> struct action < Instruction_goto_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){
      auto l = pop();
      auto &scope = parsed_items.back();
      scope->add_child(std::make_unique<instr::Goto>(create_children(std::move(l))));
    }
  };

  template<> struct action < function_rule > {
    template<typename Input>
    static void apply( const Input & in, Program & p){

      std::cout << "function\n";
      std::cout << parsed_items.size() << "\n";
      print_parsed_items();
      auto body = pop();
      auto pars = pop();
      auto name = pop();
      auto ret_type = pop();

      auto func = std::make_unique<Function>(create_children(std::move(ret_type), std::move(name), std::move(pars), std::move(body)));
      p.add_child(std::move((func)));
    }
  };

  Program parse_file (const char *fileName){

    /* 
    * Check the grammar for some possible issues.
    */
    if (pegtl::analyze< grammar >() != 0){
      std::cerr << "There are problems with the grammar" << std::endl;
      exit(1);
    }

    /*
    * Parse.
    */
    file_input< > fileInput(fileName);
    Program p;
    parse< grammar, action >(fileInput, p);

    return p;
  }
}