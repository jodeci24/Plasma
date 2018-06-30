/*==LICENSE==*

CyanWorlds.com Engine - MMOG client, server and tools
Copyright (C) 2011  Cyan Worlds, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Additional permissions under GNU GPL version 3 section 7

If you modify this Program, or any covered work, by linking or
combining it with any of RAD Game Tools Bink SDK, Autodesk 3ds Max SDK,
NVIDIA PhysX SDK, Microsoft DirectX SDK, OpenSSL library, Independent
JPEG Group JPEG library, Microsoft Windows Media SDK, or Apple QuickTime SDK
(or a modified version of those libraries),
containing parts covered by the terms of the Bink SDK EULA, 3ds Max EULA,
PhysX SDK EULA, DirectX SDK EULA, OpenSSL and SSLeay licenses, IJG
JPEG Library README, Windows Media SDK EULA, or QuickTime SDK EULA, the
licensors of this Program grant you additional
permission to convey the resulting work. Corresponding Source for a
non-source form of such a combination shall include the source code for
the parts of OpenSSL and IJG JPEG Library used as well as that of the covered
work.

You can contact Cyan Worlds, Inc. by email legal@cyan.com
 or by snail mail at:
      Cyan Worlds, Inc.
      14617 N Newport Hwy
      Mead, WA   99021

*==LICENSE==*/

#ifndef _plShaderNode_h_
#define _plShaderNode_h_

#include "plString.h"
#include <memory>
#include <vector>
#include <set>
#include <initializer_list>

enum NodeType {
    kInvalid,

    kConstant,
    kOutput,
    kOperator,
    kReturn,
    kAssignment,
    kAttribute,
    kVarying,
    kUniform,
    kTempVar,
    kArgument,
    kFnCall,
    kConditional
};


class plShaderNode : public std::enable_shared_from_this<plShaderNode> {
public:
    NodeType klass;

    plShaderNode(NodeType klass)
      : klass(klass) { }
};


class plConstantNode : public plShaderNode {
public:
    plString value;

    plConstantNode(plString value)
      : plShaderNode(kConstant),
        value(value) { }
};


class plOutputNode : public plShaderNode {
public:
    plString name;

    plOutputNode(plString name)
      : plShaderNode(kOutput),
        name(name) { }
};


// ABSTRACT
class plVariableNode : public plShaderNode {
public:
    plString name;
    plString type;

    plVariableNode(NodeType klass, plString name, plString type)
      : plShaderNode(klass),
        name(name),
        type(type) { }
};


// ABSTRACT
class plGlobalVariableNode : public plVariableNode {
public:
    plGlobalVariableNode(NodeType klass, plString name, plString type)
      : plVariableNode(klass, name, type) { }
};


class plAttributeNode : public plGlobalVariableNode {
public:
    plAttributeNode(plString name, plString type)
      : plGlobalVariableNode(kAttribute, name, type) { }
};

class plUniformNode : public plGlobalVariableNode {
public:
    plUniformNode(plString name, plString type)
      : plGlobalVariableNode(kUniform, name, type) { }
};

class plVaryingNode : public plGlobalVariableNode {
public:
    plVaryingNode(plString name, plString type)
      : plGlobalVariableNode(kVarying, name, type) { }
};


class plTempVariableNode : public plVariableNode {
public:
    plTempVariableNode(plString name, plString type)
      : plVariableNode(kTempVar, name, type) { }
};

class plArgumentNode : public plVariableNode {
public:
    int pos;

    plArgumentNode(plString name, plString type, int pos)
      : plVariableNode(kArgument, name, type),
        pos(pos) { }
};


// ABSTRACT
class plOperationNode : public plShaderNode {
public:
    std::shared_ptr<plShaderNode> lhs;
    std::shared_ptr<plShaderNode> rhs;

    plOperationNode(NodeType klass, std::shared_ptr<plShaderNode> lhs, std::shared_ptr<plShaderNode> rhs)
      : plShaderNode(klass),
        lhs(lhs),
        rhs(rhs) { }
};


class plOperatorNode : public plOperationNode {
public:
    plString op;
    bool parens;

    plOperatorNode(plString op, std::shared_ptr<plShaderNode> lhs, std::shared_ptr<plShaderNode> rhs, bool parens=false)
      : plOperationNode(kOperator, lhs, rhs),
        op(op),
        parens(parens) { }
};


class plAssignmentNode : public plOperationNode {
public:
    plAssignmentNode(std::shared_ptr<plShaderNode> lhs, std::shared_ptr<plShaderNode> rhs)
      : plOperationNode(kAssignment, lhs, rhs) { }
};


class plReturnNode : public plShaderNode {
public:
    std::shared_ptr<plShaderNode> var;

    plReturnNode(std::shared_ptr<plShaderNode> var)
      : plShaderNode(kReturn),
        var(var) { }
};


class plCallNode : public plShaderNode {
public:
    plString function;
    std::vector<std::shared_ptr<plShaderNode>> args;

    plCallNode(plString function, std::initializer_list<std::shared_ptr<plShaderNode>> args)
      : plShaderNode(kFnCall),
        function(function),
        args(args) { }
};


class plConditionNode : public plShaderNode {
public:
    std::shared_ptr<plShaderNode> condition;
    std::shared_ptr<plShaderNode> body;

    plConditionNode(std::shared_ptr<plShaderNode> condition, std::shared_ptr<plShaderNode> body)
      : plShaderNode(kConditional),
        condition(condition),
        body(body) { }
};



auto Sorter = [](std::shared_ptr<plArgumentNode> a, std::shared_ptr<plArgumentNode> b)->bool { return a->pos < b->pos; };

class plShaderFunction : public std::enable_shared_from_this<plShaderFunction>
{

    friend class plShaderContext;

    std::vector<std::shared_ptr<plShaderNode>> nodes;
    std::set<std::shared_ptr<plTempVariableNode>> temps;
    std::set<std::shared_ptr<plArgumentNode>, decltype(Sorter)> args;

    std::vector<plString> output;

public:
    plString type;
    plString name;


    plShaderFunction(plString name, plString type)
      : name(name),
        type(type),
        args(Sorter) { }

    void PushOp(std::shared_ptr<plShaderNode> op) {
        nodes.push_back(op);
    }
};




enum CtxType {
    kVertex,
    kFragment
};

class plShaderContext : public std::enable_shared_from_this<plShaderContext>
{
    std::vector<std::shared_ptr<plShaderFunction>> funcs;
    CtxType type;
    int32_t version;

    std::set<std::shared_ptr<plAttributeNode>> attributes;
    std::set<std::shared_ptr<plUniformNode>> uniforms;
    std::set<std::shared_ptr<plVaryingNode>> varyings;

public:
    plShaderContext(CtxType type, int32_t version) : type(type), version(version) { }

    void PushFunction(std::shared_ptr<plShaderFunction> fn) {
        funcs.push_back(fn);
    }

    plString Render();

private:
    plString RenderNode(std::shared_ptr<plShaderNode> node, std::shared_ptr<plShaderFunction> fn);
};


// Helper Macros
#define CONST(v)        std::make_shared<plConstantNode>(v)
#define OUTPUT(n)       std::make_shared<plOutputNode>(n)
#define ASSIGN(l, r)    std::make_shared<plAssignmentNode>(l, r)
#define ADD(...)        std::make_shared<plOperatorNode>("+", __VA_ARGS__)
#define SUB(...)        std::make_shared<plOperatorNode>("-", __VA_ARGS__)
#define MUL(...)        std::make_shared<plOperatorNode>("*", __VA_ARGS__)
#define DIV(...)        std::make_shared<plOperatorNode>("/", __VA_ARGS__)
#define PROP(n, p)      std::make_shared<plOperatorNode>(".", n, CONST(p))
#define IS_EQ(...)      std::make_shared<plOperatorNode>("==", __VA_ARGS__)
#define RETURN(n)       std::make_shared<plReturnNode>(n)
#define COND(...)       std::make_shared<plConditionNode>(__VA_ARGS__)

// This one is a bit special because of the initializer_list
#define CALL(fn, ...)   std::shared_ptr<plCallNode>(new plCallNode(fn, { __VA_ARGS__ }))

#endif
