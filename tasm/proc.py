from __future__ import absolute_import
from __future__ import print_function

import logging
import re
from builtins import object
from builtins import range
from builtins import str

from tasm import op

# ScummVM - Graphic Adventure Engine
#
# ScummVM is the legal property of its developers, whose names
# are too numerous to list here. Please refer to the COPYRIGHT
# file distributed with this source distribution.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
label_re = re.compile(r'^([\S@]+)::?(.*)$')  # speed


class Proc(object):
    last_addr = 0xc000
    elements = 1  # how many

    def __init__(self, name, far=False):
        self.name = name
        self.original_name = name
        self.__calls = []
        self.stmts = []
        self.labels = set()
        self.retlabels = set()
        self.offset = Proc.last_addr
        Proc.last_addr += 4
        # self.__line_number = line_number
        self.far = far

    def add_label(self, label, proc, line_number=0):
        self.stmts.append(op.label(label, proc, line_number=line_number))
        self.labels.add(label)

    def remove_label(self, label):
        try:
            self.labels.remove(label)
        except:
            pass
        for i in range(len(self.stmts)):
            if isinstance(self.stmts[i], op.label) and self.stmts[i].name == label:
                self.stmts[i] = op._nop(None)
                return

    def optimize_sequence(self, cls):
        i = 0
        stmts = self.stmts
        while i < len(stmts):
            if not isinstance(stmts[i], cls):
                i += 1
                continue
            if i > 0 and isinstance(stmts[i - 1], op._rep):  # skip rep prefixed instructions for now
                i += 1
                continue
            j = i + 1

            while j < len(stmts):
                if not isinstance(stmts[j], cls):
                    break
                j = j + 1

            n = j - i
            if n > 1:
                logging.info("Eliminate consequtive storage instructions at %u-%u" % (i, j))
                for k in range(i + 1, j):
                    stmts[k] = op._nop(None)
                stmts[i].repeat = n
            else:
                i = j

        i = 0
        while i < len(stmts):
            if not isinstance(stmts[i], op._rep):
                i += 1
                continue
            if i + 1 >= len(stmts):
                break
            if isinstance(stmts[i + 1], cls):
                stmts[i + 1].repeat = 'cx'
                stmts[i + 1].clear_cx = True
                stmts[i] = op._nop(None)
            i += 1
        return

    def optimize(self, keep_labels=[]):
        logging.info("optimizing...")
        # trivial simplifications
        while len(self.stmts) and isinstance(self.stmts[-1], op.label):
            logging.info("stripping last label")
            self.stmts.pop()
        '''
        #mark labels that directly precede a ret
        for i in range(len(self.stmts)):
                if not isinstance(self.stmts[i], op.label):
                        continue
                j = i
                while j < len(self.stmts) and isinstance(self.stmts[j], (op.label, op._nop)):
                        j += 1
                if j == len(self.stmts) or isinstance(self.stmts[j], op._ret):
                        logging.info "Return label: %s" % (self.stmts[i].name,)
                        self.retlabels.add(self.stmts[i].name)
        #merging push ax pop bx constructs
        i = 0
        while i + 1 < len(self.stmts):
                a, b = self.stmts[i], self.stmts[i + 1]
                if isinstance(a, op._push) and isinstance(b, op._pop):
                        ar, br = a.regs, b.regs
                        movs = []
                        while len(ar) and len(br):
                                src = ar.pop()
                                dst = br.pop(0)
                                movs.append(op._mov2(dst, src))
                        if len(br) == 0:
                                self.stmts.pop(i + 1)
                        logging.info "merging %d push-pops into movs" %(len(movs))
                        for m in movs:
                                logging.info "\t%s <- %s" %(m.dst, m.src)
                        self.stmts[i + 1:i + 1] = movs
                        if len(ar) == 0:
                                self.stmts.pop(i)
                else:
                        i += 1
        
        #eliminating unused labels
        for s in list(self.stmts):
                if not isinstance(s, op.label):
                        continue
                logging.info "checking label %s..." %s.name
                used = s.name in keep_labels
                if s.name not in self.retlabels:
                        for j in self.stmts:
                                if isinstance(j, op.basejmp) and j.label == s.name:
                                        logging.info "used"
                                        used = True
                                        break
                if not used:
                        logging.info self.labels
                        self.remove_label(s.name)

        #removing duplicate rets and rets at end
        for i in xrange(len(self.stmts)):
                if isinstance(self.stmts[i], op._ret):
                        j = i+1
                        while j < len(self.stmts) and isinstance(self.stmts[j], op._nop):
                                j += 1
                        if j == len(self.stmts) or isinstance(self.stmts[j], op._ret):
                                self.stmts[i] = op._nop(None)
        '''

        # x0r optimize speed self.optimize_sequence(op._stosb);
        # self.optimize_sequence(op._stosw);
        # self.optimize_sequence(op._movsb);
        # self.optimize_sequence(op._movsw);

    def action_instruction(self, stmt):
        # print stmt
        # comment = stmt.find(';')
        # comments = ""
        # if comment >= 0:
        #       comments = stmt[comment:]
        stmt = stmt.strip()
        line = stmt

        stmt = self.parse_extract_label(stmt)

        if len(stmt) == 0:
            return

        o = self.split_create_instruction(stmt)
        # o.line = line
        # o.line_number = line_number
        # self.stmts.append(o)
        return o

    def parse_extract_label(self, stmt):
        r = label_re.search(stmt)
        if r:
            logging.info("add label %s" % r.group(1))
            # label
            self.add_label(r.group(1).lower())
            # print "remains: %s" %r.group(2)
            stmt = r.group(2).strip()
        return stmt

    def split_create_instruction(self, stmt):
        # s = [stmt.replace("\x00", " ") for stmt in
        #     re.sub('["\'].+?["\']', lambda m: m.group(0).replace(" ", "\x00"), stmt).split()]
        s = stmt.split()
        o = self.create_instruction_object(s[0], s[1:])
        return o

    def create_instruction_object(self, instruction, args=[]):
        cl = self.find_op_class(instruction)
        # print "args %s" %s[1:]
        # arg = " ".join(args) if len(args) > 0 else ""
        o = cl(args)
        # o.comments = comments
        o.cmd = instruction.lower()
        # logging.info("~1~2~ " + o.command + " ~ " + o.cmd)
        return o

    def find_op_class(self, cmd):
        try:
            cl = getattr(op, '_' + cmd.lower())
        except AttributeError:
            cl = self.find_op_common_class(cmd)
        return cl

    def find_op_common_class(self, cmd):
        logging.info(cmd)
        if re.match(
                r"^(ins[bwd]|outs[bwd]|scas[bwd]|cmps[bwd]|movs[bwd]|salc|xlatb?|lods[bwd]|stos[bwd]|aad|repne|repe|rep|std|stc|cld|clc|cli|cbw|cwde?|cdq|sti|cmc|pushf|popf|nop|pushad?|popad?|da[as]|aa[adsm]|finit|fsin|fldz|hlt|ret[nf]?|iret|leave)$",
                cmd.lower()):
            cl = getattr(op, '_instruction0')
        elif re.match(
                r"^(dec|inc|pop|push|int|neg|div|idiv|mul|set[a-z]+|not|lods|scas|stos|cmpxchg8b|bswap|fistp|fmul|fadd|org)$",
                cmd.lower()):
            cl = getattr(op, '_instruction1')
        elif re.match(r"^(j[a-z]+|loop[a-z]*)$", cmd.lower()):
            cl = getattr(op, '_jump')
        elif re.match(
                r"^(xchg|cmp|cmpxchg|mov[sz]x|mov|or|xor|and|ad[cd]|sbb|r[oc][lr]|sub|sh[lr]|test|in|out|lea|l[defg]s|sa[rl]|bt[rsc]?|movs|xadd|cmov[a-z]+|enter|bs[rf])$",
                cmd.lower()):
            cl = getattr(op, '_instruction2')
        elif re.match(r"^(shrd|shld)$", cmd.lower()):
            cl = getattr(op, '_instruction3')
        else:
            raise Exception("unknown command: " + cmd.lower())
        return cl

    def add_equ(self, label, value, line_number=0):  # only for tests. to remove
        o = self.add_equ_(label, value, line_number)
        self.stmts.insert(0, o)

    def add_equ_(self, label, value, line_number):  # TODO Move it to parser
        import tasm.cpp
        o1 = op._equ(label, value)
        # print "args %s" %s[1:]
        value = tasm.cpp.convert_number_to_c(value)
        # o1 = cl(label, value)
        o1.line = str(line_number) + " " + label + " equ " + value
        o1.cmd = o1.line
        #               logging.info "~~~" + o.command + o.comments
        return o1

    def add_assignment(self, label, value, line_number=0):
        import tasm.cpp
        # print "args %s" %s[1:]
        logging.info(label + " " + value)
        value = tasm.cpp.convert_number_to_c(value)
        o = op._assignment(label, value)
        o.line = str(line_number) + " " + label + " = " + value
        o.cmd = o.line

        #               logging.info "~~~" + o.command + o.comments
        return o

    def __str__(self):
        r = []
        for i in self.stmts:
            r.append(i.__str__())
        return "\n".join(r)

    def visit(self, visitor, skip=0):
        for i in range(skip, len(self.stmts)):
            stmt = self.stmts[i]
            s = self.generate_c_cmd(visitor, stmt)
            visitor.body += s
            try:  # trying to add command and comment
                if stmt.line or stmt.line_number != 0:
                    visitor.body = visitor.body[:-1] + "\t// " + str(stmt.line_number) \
                                   + " " + stmt.line + "\n"
            except AttributeError:
                pass

    def generate_c_cmd(self, visitor, stmt):
        s = stmt.visit(visitor)
        return s
