//
// Created by thinh on 02/04/2021.
//

#include "Contact.h"

Contact::Contact(std::string contactType, std::vector<std::string> contactClasses, std::vector<std::string> contactRates) {
    this->contactType = contactType;
    for (std::string contactClass: contactClasses) {
        this->contactClasses.push_back(contactClass);
    }
    for (std::string contactRate: contactRates) {
        this->contactRates.push_back(contactRate);
    }
}

std::string Contact::getContactType() {
    return contactType;
}

std::vector<std::string> Contact::getContactClasses() {
    return contactClasses;
}

std::vector<std::string> Contact::getContactRates() {
    return contactRates;
}