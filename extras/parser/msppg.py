#!/usr/bin/python3
'''
Multiwii Serial Protocol Parser Generator

Copyright (C) Rob Jones, Alec Singer, Chris Lavin, Blake Liebling,
Simon D. Levy 2021

MIT License
'''

import os
import json
import argparse


# Code-emitter classes ========================================================


class CodeEmitter(object):

    def __init__(self, msgdict, typevals):

        self.msgdict = msgdict
        typenames = ('byte', 'short', 'float', 'int')
        self.typedict = {n:t  for n, t in zip(typenames, typevals)}
        self.sizedict = {n:t  for n, t in zip(typenames, (1,2,3,4))}

    @staticmethod
    def clean(string):
        cleaned_string = string[1: len(string) - 1]
        return cleaned_string

    def _openw(self, fname):

        fname = 'output/' + fname
        print('Creating file ' + fname)
        return open(fname, 'w')

    def _paysize(self, argtypes):

        return sum([self.sizedict[atype] for atype in argtypes])

    def _msgsize(self, argtypes):

        return self._paysize(argtypes)

    def _getargnames(self, message):

        return [argname for (argname, _) in self._getargs(message)]

    def _getargtypes(self, message):

        return [argtype for (_, argtype) in self._getargs(message)]

    def _getargs(self, message):

        return [(argname, argtype) for (argname, argtype) in
                zip(message[1], message[2]) if argname.lower() != 'comment']

    def _write_params(self, outfile, argtypes, argnames, prefix='(',
                      ampersand=''):

        outfile.write(prefix)
        for argtype, argname in zip(argtypes, argnames):
            outfile.write(self.typedict[argtype] + ' ' + ampersand + ' ' +
                          argname)
            if argname != argnames[-1]:
                outfile.write(', ')
        outfile.write(')')


# C++ emitter =================================================================


class Cpp_Emitter(CodeEmitter):

    def __init__(self, msgdict):

        CodeEmitter.__init__(self, msgdict,
                             ('uint8_t', 'int16_t', 'float', 'int32_t'))

    def emit(self):

        # Open output file
        output = self._openw('serialtask.hpp')

        # Write header
        output.write('/*\n')
        output.write('  Timer task for serial comms\n\n')
        output.write('  MIT License\n')
        output.write('*/\n\n')
        output.write('#pragma once\n\n')
        output.write('#include <RFT_timertask.hpp>\n')
        output.write('#include <RFT_board.hpp>\n')
        output.write('#include <RFT_debugger.hpp>\n')
        output.write('#include <RFT_actuator.hpp>\n')
        output.write('#include <RFT_serialtask.hpp>\n')
        output.write('#include <RFT_parser.hpp>\n\n')

        # Add optional namespace
        output.write('namespace XXX {\n\n')

        # Add classname
        output.write('\nclass SerialTask {')

        # Add stubbed declarations for handler methods

        output.write('\n\n    private:\n\n')

        for msgtype in self.msgdict.keys():

            msgstuff = self.msgdict[msgtype]
            msgid = msgstuff[0]

            argnames = self._getargnames(msgstuff)
            argtypes = self._getargtypes(msgstuff)

            output.write('        void handle_%s%s' %
                         (msgtype, '_Request' if msgid < 200 else ''))
            self._write_params(output, argtypes, argnames,
                               ampersand=('&' if msgid < 200 else ''))
            output.write(' { \n            // XXX YOUR CODE HERE\n')
            output.write('        }\n\n')

        # Add dispatchMessage() method

        output.write('    protected:\n\n')
        output.write('        void dispatchMessage(void) override\n')
        output.write('        {\n')
        output.write('            switch (_command) {\n\n')

        for msgtype in self.msgdict.keys():

            msgstuff = self.msgdict[msgtype]
            msgid = msgstuff[0]

            argnames = self._getargnames(msgstuff)
            argtypes = self._getargtypes(msgstuff)

            output.write('                case %s: {\n' % self.msgdict[msgtype][0])
            nargs = len(argnames)
            offset = 0
            for k in range(nargs):
                argname = argnames[k]
                argtype = argtypes[k]
                decl = self.typedict[argtype]
                output.write('                    ' + decl + ' ' + argname +
                             ' = 0;\n')
                if msgid >= 200:
                    fmt = 'memcpy(&%s,  &_inBuf[%d], sizeof(%s));\n\n'
                    output.write(' '*20 + fmt % (argname, offset, decl))
                offset += self.sizedict[argtype]
            output.write('                    handle_%s%s(' %
                         (msgtype, '_Request' if msgid < 200 else ''))
            for k in range(nargs):
                output.write(argnames[k])
                if k < nargs-1:
                    output.write(', ')
            output.write(');\n')
            if msgid < 200:
                # XXX enforce uniform type for now
                argtype = argtypes[0].capitalize()
                output.write('                    prepareToSend%ss(%d);\n' %
                             (argtype, nargs))
                for argname in argnames:
                    output.write('                    send%s(%s);\n' %
                                 (argtype, argname))
                output.write('                    serialize8(_checksum);\n')
            output.write('                } break;\n\n')

        output.write('            }\n\n')
        output.write('        } // dispatchMessage \n\n')
        output.write('    }; // class SerialTask\n\n')
        output.write('} // namespace XXX\n')


# Python emitter ==============================================================


class Python_Emitter(CodeEmitter):

    def __init__(self, msgdict):

        CodeEmitter.__init__(self, msgdict, ('B' ,'h', 'f', 'i'))

        self.type2pack = {'byte': 'B',
                          'short': 'h',
                          'float': 'f',
                          'int': 'i'}


    def emit(self):

        # Open output file
        self.output = self._openw('myparser.py')

        # Write header
        self.output.write('#  Message dispatcher')
        self.output.write('\n\n#  MIT License')
        self._write('\n\nimport struct')
        self._write('\nfrom msppg import Parser')
        self._write('\n\n\nclass MyParser(Parser):')
        self._write('\n\n    def dispatchMessage(self):')

        for msgtype in self.msgdict.keys():
            msgstuff = self.msgdict[msgtype]
            msgid = msgstuff[0]
            if msgid < 200:
                self._write('\n\n        if self.message_id == %d:\n'
                            % msgstuff[0])
                self._write('            self.handle_%s(*struct.unpack(\'=' %
                            msgtype)
                for argtype in self._getargtypes(msgstuff):
                    self._write('%s' % self.type2pack[argtype])
                self._write("\'" + ', self.message_buffer))')

        # Emit handler methods for parser
        for msgtype in self.msgdict.keys():

            msgstuff = self.msgdict[msgtype]
            msgid = msgstuff[0]
            if msgid < 200:
                self._write('\n\n    def handle_%s(self' % msgtype)
                for argname in self._getargnames(msgstuff):
                    self._write(', ' + argname)
                self._write('):\n')
                self._write('        # XXX YOUR CODE HERE\n')
                self._write('        return')

        # Emit serializer functions for module
        for msgtype in self.msgdict.keys():

            msgstuff = self.msgdict[msgtype]
            msgid = msgstuff[0]

            if msgid < 200:

                self._write('\n\n\ndef serialize_' + msgtype + '_Request():\n')
                self._write(('    msg = \'$M<\' + chr(0) + '
                            'chr(%s) + chr(%s)\n') % (msgid, msgid))
                self._write('    return bytes(msg, \'utf-8\')')

            else:

                self._write('\n\n\ndef serialize_' + msgtype +
                            '(' + ', '.join(self._getargnames(msgstuff)) +
                            '):\n')
                self._write('    message_buffer = struct.pack(\'')
                for argtype in self._getargtypes(msgstuff):
                    self._write(self.type2pack[argtype])
                self._write('\'')
                for argname in self._getargnames(msgstuff):
                    self._write(', ' + argname)
                self._write(')\n')

                self._write(('    msg = [len(message_buffer), %s] + ' +
                            'list(message_buffer)\n') % msgid)
                self._write('    return bytes([ord(\'$\'), ord(\'M\'), ' +
                            'ord(\'<\')] + msg + [Parser.crc8(msg)])')
        self._write('\n')

    def _write(self, s):

        self.output.write(s)

# Java emitter ================================================================


class Java_Emitter(CodeEmitter):

    def __init__(self, msgdict):

        CodeEmitter.__init__(self, msgdict, ('byte', 'short', 'float', 'int'))

        self.bbdict = {'byte': '',
                        'short': 'Short',
                        'float': 'Float',
                        'int': 'Int'}

    def emit(self):

        self.output = self._openw('MyParser.java')

        # Write header
        self.output.write('/*\n')
        self.output.write('   Message dispatcher\n\n')
        self.output.write('   MIT License\n\n')
        self.output.write('*/\n\n')
        self._write('import edu.wlu.cs.mssppg.Parser;\n\n')
        self._write('public class MyParser extends Parser {\n\n')
        self._write('    protected void dispatchMessage(void) {\n\n')
        self._write('        switch (_command) {\n\n')

        # Write handler cases for incoming messages
        for msgtype in self.msgdict.keys():

            msgstuff = self.msgdict[msgtype]
            msgid = msgstuff[0]

            if msgid < 200:

                self._write('            case (byte)%d:\n' % msgid)
                self._write('                this.handle_%s(\n' % msgtype)

                argnames = self._getargnames(msgstuff)
                argtypes = self._getargtypes(msgstuff)

                nargs = len(argnames)

                offset = 0
                for k in range(nargs):
                    argtype = argtypes[k]
                    self._write('                        bb.get%s(%d)' %
                                (self.bbdict[argtype], offset))
                    offset += self.sizedict[argtype]
                    if k < nargs-1:
                        self._write(',\n')
                self._write(');\n')

                self._write('                break;\n\n')

        self._write('        }\n    }\n\n')

        for msgtype in self.msgdict.keys():

            msgstuff = self.msgdict[msgtype]
            msgid = msgstuff[0]

            argnames = self._getargnames(msgstuff)
            argtypes = self._getargtypes(msgstuff)

            # For messages from FC
            if msgid < 200:

                # Write serializer for requests
                self._write(('    public byte [] serialize_%s_Request() ' +
                            '{\n\n') % msgtype)
                self._write('        byte [] message = new byte[6];\n\n')
                self._write('        message[0] = 36;\n')
                self._write('        message[1] = 77;\n')
                self._write('        message[2] = 60;\n')
                self._write('        message[3] = 0;\n')
                self._write('        message[4] = (byte)%d;\n' % msgid)
                self._write('        message[5] = (byte)%d;\n\n' % msgid)
                self._write('        return message;\n')
                self._write('    }\n\n')

                # Write handler for replies from flight controller
                self._write('    protected void handle_%s' % msgtype)
                self._write_params(self.output, argtypes, argnames)
                self._write(' { \n        // XXX YOUR CODE HERE\n    }\n\n')

        self._write('}\n')

    def _write(self, s):

        self.output.write(s)

# main ========================================================================


def main():

    # parse file name from command line
    parser = argparse.ArgumentParser()
    parser.add_argument('--infile', type=str, required=False,
                        default='messages.json',
                        help='Random seed for reproducibility')
    args = parser.parse_args()

    data = json.load(open(args.infile, 'r'))

    # takes the types of messages from the json file
    unicode_message_types = data.keys()

    # make a list of messages from the JSON file
    message_type_list = list()
    for key in unicode_message_types:
        message_type = json.dumps(key)
        clean_type = CodeEmitter.clean(message_type)
        message_type_list.append(clean_type)

    # make dictionary of names, types for each message's components
    argument_lists = list()
    argument_types = list()
    msgdict = {}
    for msgtype in message_type_list:
        argnames = list()
        argtypes = list()
        msgid = None
        for arg in data[msgtype]:
            argname = CodeEmitter.clean(CodeEmitter.clean(
                json.dumps(list(arg.keys()))))
            argtype = arg[list(arg.keys())[0]]
            if argname == 'ID':
                msgid = int(argtype)
            else:
                argtypes.append(argtype)
                argnames.append(argname)
            argument_lists.append(argnames)
        if msgid is None:
            print('Missing ID for message ' + msgtype)
            exit(1)
        argument_types.append(argtypes)
        msgdict[msgtype] = (msgid, argnames, argtypes)

    # Create output directory
    os.makedirs('output/', exist_ok=True)

    # Emit C++
    Cpp_Emitter(msgdict).emit()

    # Emit Python
    Python_Emitter(msgdict).emit()

    # Emite Java
    Java_Emitter(msgdict).emit()


if __name__ == '__main__':
    main()
