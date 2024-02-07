#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"
unsigned int assigned_trans_id = 50000;
////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//

// Constructor
Transaction::Transaction(std::string ticker_symbol, unsigned int day_date, unsigned int month_date, unsigned year_date, bool buy_sell_trans, unsigned int number_shares, double trans_amount)
    : symbol(ticker_symbol), day(day_date), month(month_date), year(year_date),
      trans_type(buy_sell_trans ? "Buy" : "Sell"), shares(number_shares), amount(trans_amount),
      trans_id(assigned_trans_id++), p_next(nullptr) {
    trans_type = buy_sell_trans ? "Buy" : "Sell";
    trans_id = assigned_trans_id++;
    acb = 0;
    acb_per_share = 0;
    share_balance = 0;
    cgl = 0;
    p_next = nullptr;
}

// Destructor
Transaction::~Transaction() {
    
}

// Overloaded < operator
bool Transaction::operator<(const Transaction &other) {
    if (year != other.year) 
        return year < other.year;
    if (month != other.month) 
        return month < other.month;
    if (day != other.day)
        return day < other.day;
    
    return trans_id < other.trans_id;
}




// GIVEN
// Member functions to get values.
//
std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values.
//
void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
    << std::setw(4) << get_symbol() << " "
    << std::setw(4) << get_day() << " "
    << std::setw(4) << get_month() << " "
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) {
    std::cout << "  Buy  ";
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " "
    << std::setw(10) << get_amount() << " "
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl()
    << std::endl;
} 

////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//


// Constructor
History::History() {
    p_head = nullptr;
}

// Destructor
History::~History() {
    Transaction* current = p_head;
    while (current != nullptr) {
        Transaction* temp = current->get_next(); 
        delete current; 
        current = temp; 
    }
}

// insert(...): Insert transaction into linked list.
// TODO
void History::insert(Transaction* p_new_trans) {
    if (p_new_trans == nullptr) {
        return; 
    }

    
    p_new_trans->set_next(nullptr);

    if (p_head == nullptr) {
   
        p_head = p_new_trans;
    } else {
        
        Transaction* current = p_head;
        while (current->get_next() != nullptr) {
            current = current->get_next();
        }
        current->set_next(p_new_trans); 
    }
}



// read_history(...): Read the transaction history from file.
// TODO
void History::read_history() {
    ece150::open_file(); 

    while (ece150::next_trans_entry()) {
        std::string symbol = ece150::get_trans_symbol();
        unsigned int day = ece150::get_trans_day();
        unsigned int month = ece150::get_trans_month();
        unsigned int year = ece150::get_trans_year();
        bool type = ece150::get_trans_type(); 
        unsigned int shares = ece150::get_trans_shares();
        double amount = ece150::get_trans_amount();

        Transaction* new_transaction = new Transaction(symbol, day, month, year, type, shares, amount);
        insert(new_transaction);
    }

    ece150::close_file();
}

// print() Print the transaction history.
// TODO
void History::print() {
    std::cout << "BEGIN TRANSACTION HISTORY" << std::endl;

    Transaction* current = p_head;
    while (current != nullptr) {
        current->print(); 
        current = current->get_next();
    }

    std::cout << "END TRANSACTION HISTORY" << std::endl;
}

// sort_by_date(): Sort the linked list by trade date.
// TODO
void History::sort_by_date() {
    if (p_head == nullptr || p_head->get_next() == nullptr) {
        return;
    }

    Transaction *sorted = nullptr;

    while (p_head != nullptr) {
        Transaction *current = p_head;
        p_head = p_head->get_next();

        if (sorted == nullptr || *current < *sorted) {
            current->set_next(sorted);
            sorted = current;
        } else {
            Transaction *temp = sorted;
            while (temp->get_next() != nullptr && !(*current < *temp->get_next())) {
                temp = temp->get_next();
            }
            current->set_next(temp->get_next());
            temp->set_next(current);
        }
    }

    p_head = sorted;
}


// update_acb_cgl(): Updates the ACB and CGL values.
// TODO
void History::update_acb_cgl() {
    double total_acb = 0.0;
    unsigned int total_shares = 0;

    for (Transaction *current = p_head; current != nullptr; current = current->get_next()) {
        if (current->get_trans_type()) { 
            total_acb += current->get_amount();
            total_shares += current->get_shares();
        } else { 
            double acb_per_share = total_acb / total_shares;
            total_acb -= acb_per_share * current->get_shares();
            current->set_cgl(current->get_amount() - acb_per_share * current->get_shares());
            total_shares -= current->get_shares();
        }
        current->set_acb(total_acb);
        current->set_acb_per_share(total_acb / total_shares);
        current->set_share_balance(total_shares);
    }
}

// compute_cgl(): )Compute the ACB, and CGL.
// TODO
double History::compute_cgl(unsigned int year) {
    double total_cgl = 0.0;

    for (Transaction *current = p_head; current != nullptr; current = current->get_next()) {
        if (current->get_year() == year) {
            total_cgl += current->get_cgl();
        }
    }

    return total_cgl;
}


// GIVEN
// get_p_head(): Full access to the linked list.
//
Transaction *History::get_p_head() { return p_head; }
