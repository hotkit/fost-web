/**
    Copyright 2016-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/fsigma.hpp>
#include <fost/push_back>


/**
    ## fsigma::frame
 */


fsigma::frame::frame(frame *f) : parent(f) {}


fostlib::json fsigma::frame::argument(
        const fostlib::string &name,
        fostlib::json::const_iterator &pos,
        fostlib::json::const_iterator end) {
    if (pos == end) {
        throw fostlib::exceptions::not_implemented(
                __PRETTY_FUNCTION__, "Argument not found", name);
    } else {
        auto result = *pos;
        ++pos;
        return result;
    }
}


fostlib::string fsigma::frame::resolve_string(const fostlib::json &code) {
    if (code.isatom()) {
        return fostlib::coerce<fostlib::string>(code);
    } else if (code.isarray()) {
        frame stack(this);
        return resolve_string(call(stack, code));
    } else {
        throw fostlib::exceptions::not_implemented(
                __PRETTY_FUNCTION__, "Can't resolve to a string", code);
    }
}


int64_t fsigma::frame::resolve_int(const fostlib::json &code) {
    if (code.isatom()) {
        return fostlib::coerce<int64_t>(code);
    } else {
        throw fostlib::exceptions::not_implemented(
                __PRETTY_FUNCTION__, "Can't resolve to an int", code);
    }
}


fostlib::json fsigma::frame::resolve(const fostlib::json &code) {
    /// S-expressions are always a JSON array. Everything else is a literal
    /// and doesn't need to be resolved.
    if (code.isarray()) {
        frame stack(this);
        return call(stack, code);
    } else {
        return code;
    }
}


/// This is dynamic rather than lexical scoping, which is.... not great
fostlib::json fsigma::frame::lookup(const fostlib::string &name) const {
    auto fnp = symbols.find(name);
    if (fnp == symbols.end()) {
        if (parent) {
            return parent->lookup(name);
        } else {
            throw fostlib::exceptions::not_implemented(
                    __PRETTY_FUNCTION__, "Sumbol not found", name);
        }
    } else {
        return fnp->second;
    }
}


/// This is dynamic rather than lexical scoping, which is.... not great
fsigma::frame::builtin
        fsigma::frame::lookup_function(const fostlib::string &name) const {
    auto fnp = native.find(name);
    if (fnp == native.end()) {
        if (parent) {
            return parent->lookup_function(name);
        } else {
            throw fostlib::exceptions::not_implemented(
                    __PRETTY_FUNCTION__, "Function not found", name);
        }
    } else {
        return fnp->second;
    }
}


/**
    ## fsigma::call
 */


fostlib::json fsigma::call(frame &stack, const fostlib::json &sexpr) {
    if (not sexpr.isarray()) {
        throw fostlib::exceptions::not_implemented(
                __PRETTY_FUNCTION__, "Script isn't an array/s-expression",
                sexpr);
    } else if (sexpr.size() == 0) {
        throw fostlib::exceptions::not_implemented(
                __PRETTY_FUNCTION__, "The script was empty");
    } else {
        return call(
                stack, stack.resolve_string(*sexpr.begin()), ++sexpr.begin(),
                sexpr.end());
    }
}


fostlib::json fsigma::call(
        frame &stack,
        const fostlib::string &name,
        fostlib::json::const_iterator begin,
        fostlib::json::const_iterator end) {
    try {
        frame::builtin function(stack.lookup_function(name));
        return function(stack, begin, end);
    } catch (fostlib::exceptions::exception &e) {
        // Built a stack frame
        fostlib::json sf;
        fostlib::push_back(sf, name);
        for (auto iter = begin; iter != end; ++iter) {
            fostlib::push_back(sf, *iter);
        }
        // Add to the back trace
        fostlib::push_back(e.data(), "fg", "backtrace", sf);
        throw;
    }
}
