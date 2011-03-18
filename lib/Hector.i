%module Hector

%{
/* This is needed for Perl to compile */
#ifdef SWIGPERL
#undef New // protocol buffers + SWIG interaction
#undef die // Perl
#endif

/* This is needed for Python to compile */
#ifdef SWIGPYTHON
#define SWIG_FILE_WITH_INIT // for Python
#endif
%}

%include "std_vector.i"
%include "std_string.i"
%template(StringVector_hector) std::vector<std::string>;

%include Connection.i
%include StdioConnection.i
%include IpAddr.i
%include Object.i
%include ObjectRegistry.i
%include Module.i
%include ResourceRegistry.i
%include Resource.i
%include ProtobufResource.i
%include PerlResource.i
%include TestResource.i
%include TestProtobufResource.i
%include ResourceAttrInfo.i

%{
/* We need some local symbols to be exported, so we wrap them */
#ifdef SWIGPERL
extern "C" swig_type_info *Perl_TypeQuery_Wrapper(const char *ty) {
        return SWIG_TypeQuery(ty);
}
extern "C" SV *Perl_NewPointerObj_Wrapper(void *ptr, swig_type_info *ty, int flags) {
        return SWIG_NewPointerObj(ptr, ty, flags);
}
extern "C" int Perl_ConvertPtr_Wrapper(SV *obj, void **ptr, swig_type_info *ty, int flags) {
        return SWIG_ConvertPtr(obj, ptr, ty, flags);
}
#endif

#ifdef SWIGPYTHON
extern "C" swig_type_info *Python_TypeQuery_Wrapper(const char *ty) {
        return SWIG_TypeQuery(ty);
}
extern "C" PyObject *Python_NewPointerObj_Wrapper(void *ptr, swig_type_info *ty, int flags) {
        return SWIG_NewPointerObj(ptr, ty, flags);
}
extern "C" int Python_ConvertPtr_Wrapper(PyObject *obj, void **ptr, swig_type_info *ty, int flags) {
        return SWIG_ConvertPtr(obj, ptr, ty, flags);
}
#endif
%}
