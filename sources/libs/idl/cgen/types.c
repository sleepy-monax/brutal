#include <brutal/debug.h>
#include <cc/builder.h>
#include <idl/builtin.h>
#include <idl/cgen.h>

CType idl_cgen_decl_primitive(IdlType type)
{
    assert_truth(type.type == IDL_TYPE_PRIMITIVE);

    IdlBuiltinType *builtin = idl_lookup_builtin(type.primitive_.name);

    if (builtin != nullptr)
    {
        return ctype_ident(builtin->cname);
    }

    return ctype_ident(type.primitive_.mangled);
}

CType idl_cgen_decl_enum(IdlType type, Alloc *alloc)
{
    CType ctype = ctype_enum(alloc);

    int i = 0;
    vec_foreach_v(member, &type.enum_.members)
    {
        ctype_constant(&ctype, member.mangled, cval_signed(i++));
    }

    return ctype;
}

CType idl_cgen_decl_struct(IdlType type, Alloc *alloc)
{
    CType ctype = ctype_struct(alloc);

    vec_foreach_v(member, &type.struct_.members)
    {
        ctype_member(&ctype, member.name, idl_cgen_decl_type(member.type, alloc));
    }

    return ctype;
}

CType idl_cgen_decl_vec(IdlType type, Alloc *alloc)
{
    CType ctype = ctype_struct(alloc);
    CType buf_type = ctype_ptr(idl_cgen_decl_type(*type.vec_.subtype, alloc), alloc);
    CType size_type = ctype_ident(str$("size_t"));

    ctype_member(&ctype, str$("buf"), buf_type);
    ctype_member(&ctype, str$("len"), size_type);

    return ctype;
}

CType idl_cgen_decl_type(IdlType type, Alloc *alloc)
{
    switch (type.type)
    {
    case IDL_TYPE_NIL:
        return ctype_void();

    case IDL_TYPE_PRIMITIVE:
        return idl_cgen_decl_primitive(type);

    case IDL_TYPE_ENUM:
        return idl_cgen_decl_enum(type, alloc);

    case IDL_TYPE_STRUCT:
        return idl_cgen_decl_struct(type, alloc);

    case IDL_TYPE_VEC:
        return idl_cgen_decl_vec(type, alloc);

    default:
        panic$("Unknow type type {}", type.type);
    }
}