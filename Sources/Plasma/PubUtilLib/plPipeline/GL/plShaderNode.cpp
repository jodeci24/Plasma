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

#include "plShaderNode.h"
#include "plFormat.h"

using namespace std;

plString plShaderContext::RenderNode(std::shared_ptr<plShaderNode> node, std::shared_ptr<plShaderFunction> fn) {
    switch (node->klass)
    {
        case kConstant:
        {
            std::shared_ptr<plConstantNode> c = static_pointer_cast<plConstantNode>(node);

            return c->value;
        }
        break;

        case kAttribute:
        {
            std::shared_ptr<plAttributeNode> attr = static_pointer_cast<plAttributeNode>(node);

            this->attributes.insert(attr);

            return attr->name;
        }
        break;

        case kUniform:
        {
            std::shared_ptr<plUniformNode> uni = static_pointer_cast<plUniformNode>(node);

            this->uniforms.insert(uni);

            return uni->name;
        }
        break;

        case kVarying:
        {
            std::shared_ptr<plVaryingNode> var = static_pointer_cast<plVaryingNode>(node);

            this->varyings.insert(var);

            return var->name;
        }
        break;

        case kTempVar:
        {
            std::shared_ptr<plTempVariableNode> tmp = static_pointer_cast<plTempVariableNode>(node);

            fn->temps.insert(tmp);

            return tmp->name;
        }
        break;

        case kArgument:
        {
            std::shared_ptr<plArgumentNode> arg = static_pointer_cast<plArgumentNode>(node);

            fn->args.insert(arg);

            return arg->name;
        }
        break;

        case kOutput:
        {
            std::shared_ptr<plOutputNode> out = static_pointer_cast<plOutputNode>(node);

            return out->name;
        }
        break;

        case kOperator:
        {
            std::shared_ptr<plOperatorNode> op = static_pointer_cast<plOperatorNode>(node);

            if (op->op == ".") {
              return plFormat("{}{}{}", this->RenderNode(op->lhs, fn), op->op, this->RenderNode(op->rhs, fn));
            } else if (op->op == "[") {
              return plFormat("{}{}{}]", this->RenderNode(op->lhs, fn), op->op, this->RenderNode(op->rhs, fn));
            } else if (op->parens) {
              return plFormat("({} {} {})", this->RenderNode(op->lhs, fn), op->op, this->RenderNode(op->rhs, fn));
            } else {
              return plFormat("{} {} {}", this->RenderNode(op->lhs, fn), op->op, this->RenderNode(op->rhs, fn));
            }
        }
        break;

        case kAssignment:
        {
            std::shared_ptr<plAssignmentNode> asn = static_pointer_cast<plAssignmentNode>(node);

            return plFormat("{} = {}", this->RenderNode(asn->lhs, fn), this->RenderNode(asn->rhs, fn));
        }
        break;

        case kReturn:
        {
            std::shared_ptr<plReturnNode> ret = static_pointer_cast<plReturnNode>(node);

            return plFormat("return {}", this->RenderNode(ret->var, fn));
        }
        break;

        case kFnCall:
        {
            std::shared_ptr<plCallNode> call = static_pointer_cast<plCallNode>(node);

            std::vector<plString> params;

            for (std::shared_ptr<plShaderNode> arg : call->args) {
                params.push_back(this->RenderNode(arg, fn));
            }

            plStringStream out;
            out << call->function << "(";

            for (size_t i = 0; i < call->args.size(); i++) {
                if (i > 0) {
                    out << ", ";
                }
                out << params[i];
            }
            out << ")";

            return out.GetString();
        }
        break;

        case kConditional:
        {
            std::shared_ptr<plConditionNode> cond = static_pointer_cast<plConditionNode>(node);

            plStringStream out;
            out << plFormat("if ({}) {{", this->RenderNode(cond->condition, fn));

            if (cond->nodes.size() == 1) {
                out << plFormat(" {}; }", this->RenderNode(cond->nodes[0], fn));
            } else {
                out << "\n";

                for (size_t i = 0; i < cond->nodes.size(); i++) {
                    out << "\t" << this->RenderNode(cond->nodes[i], fn) << ";\n";
                }

                out << "}";
            }

            return out.GetString();
        }
        break;

        default:
        return "";
    }
}

plString plShaderContext::Render()
{
    std::vector<plString> lines;

    for (std::shared_ptr<plShaderFunction> fn : this->funcs) {
        for (std::shared_ptr<plShaderNode> node : fn->nodes) {
            fn->output.push_back(this->RenderNode(node, fn));
        }
    }


    plStringStream out;

    out << plFormat("#version {}\n", this->version);

    if (this->type == kFragment) {
        out << "precision mediump float;\n";
    }

    for (std::shared_ptr<plShaderStruct> st : this->structs) {
        out << plFormat("struct {} {{\n", st->name);

        for (std::shared_ptr<plVariableNode> var : st->fields) {
            if (var->count > 1) {
                out << "\t" << plFormat("{} {}[{}];\n", var->type, var->name, var->count);
            } else {
                out << "\t" << plFormat("{} {};\n", var->type, var->name);
            }
        }

        out << "};\n";
    }

    for (std::shared_ptr<plAttributeNode> node : this->attributes) {
        if (node->count > 1) {
            out << plFormat("attribute {} {}[{}];\n", node->type, node->name, node->count);
        } else {
            out << plFormat("attribute {} {};\n", node->type, node->name);
        }
    }

    for (std::shared_ptr<plUniformNode> node : this->uniforms) {
        if (node->count > 1) {
            out << plFormat("uniform {} {}[{}];\n", node->type, node->name, node->count);
        } else {
            out << plFormat("uniform {} {};\n", node->type, node->name);
        }
    }

    for (std::shared_ptr<plVaryingNode> node : this->varyings) {
        if (node->count > 1) {
            out << plFormat("varying {} {}[{}];\n", node->type, node->name, node->count);
        } else {
            out << plFormat("varying {} {};\n", node->type, node->name);
        }
    }


    for (std::shared_ptr<plShaderFunction> fn : this->funcs) {
        out << plFormat("\n{} {}(", fn->type, fn->name);

        size_t i = 0;
        for (std::shared_ptr<plArgumentNode> arg : fn->args) {
            if (i > 0) {
                out << ", ";
            }
            out << plFormat("{} {}", arg->type, arg->name);
            i++;
        }

        out << ") {\n";


        for (std::shared_ptr<plTempVariableNode> node : fn->temps) {
            if (node->count > 1) {
                out << "\t" << plFormat("{} {}[{}];\n", node->type, node->name, node->count);
            } else {
                out << "\t" << plFormat("{} {};\n", node->type, node->name);
            }
        }

        if (fn->temps.size()) {
            out << "\n";
        }

        for (plString ln : fn->output) {
            out << "\t" << ln << ";\n";
        }

        out << "}\n";
    }

    return out.GetString();
}
