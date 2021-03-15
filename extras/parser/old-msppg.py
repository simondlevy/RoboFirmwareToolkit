#!/usr/bin/python3
'''
Multiwii Serial Protocol Parser Generator

Copyright (C) Rob Jones, Alec Singer, Chris Lavin, Blake Liebling,
Simon D. Levy 2021

MIT License
'''

from sys import exit
import os
import json
from pkg_resources import resource_string
import argparse


# Helper functions ============================================================


def clean(string):
    cleaned_string = string[1: len(string) - 1]
    return cleaned_string


def error(errmsg):
    print(errmsg)
    exit(1)


def _openw(fname):

    print('Creating file ' + fname)
    return open(fname, 'w')

# Code-emitter classes ========================================================


class CodeEmitter(object):

    def __init__(self):

        self.indent = '    '
        self.type2size = {'byte': 1, 'short': 2, 'float': 4, 'int': 4}

    def _paysize(self, argtypes):

        return sum([self.type2size[atype] for atype in argtypes])

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
            outfile.write(self.type2decl[argtype] + ' ' + ampersand + ' ' +
                          argname)
            if argname != argnames[-1]:
                outfile.write(', ')
        outfile.write(')')

    def _getsrc(self, filename):

        return resource_string('resources', filename).decode('utf-8')

    def _copyfile(self, src, dst, folder='output'):

        outfile = _openw('%s/%s' % (folder, dst))
        outfile.write(self._getsrc(src))
        outfile.close()


class LocalCodeEmitter(CodeEmitter):

    def __init__(self, folder, ext):

        CodeEmitter.__init__(self)


class CompileableCodeEmitter(LocalCodeEmitter):

    def __init__(self, folder, ext):

        LocalCodeEmitter.__init__(self, folder, ext)

        self._copyfile('%s.makefile' % folder, '%s/Makefile' % folder)

# Python emitter ==============================================================


class Python_Emitter(LocalCodeEmitter):

    def __init__(self, msgdict):

        LocalCodeEmitter.__init__(self, 'python', 'py')

        self.output = _openw('output/python/msppg/myparser.py')

        self.type2pack = {'byte': 'B',
                          'short': 'h',
                          'float': 'f',
                          'int': 'i'}

        # Write header
        self.output.write('#  Message dispatcher\n\n')
        self.output.write('#  MIT License\n\n')
        self._write('from msppg import Parser\n\n')
        self._write('class MyParser(Parser):\n\n')
        self._write('    def dispatchMessage(self):\n\n')

        for msgtype in msgdict.keys():
            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]
            self._write('        if self.message_id == %d:\n\n' % msgstuff[0])
            self._write('            if self.message_direction == 0:\n\n')
            self._write('                self.handle_%s_Request()\n\n' %
                        msgtype)
            self._write('         else:\n\n')
            self._write('            self.handle_%s(*struct.unpack(\'=' %
                        msgtype)
            for argtype in self._getargtypes(msgstuff):
                self._write('%s' % self.type2pack[argtype])
            self._write("\'" + ', self.message_buffer))\n\n')

        # Emit handler methods for parser
        for msgtype in msgdict.keys():

            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]

            self._write('    def handle_%s(self' % msgtype)
            for argname in self._getargnames(msgstuff):
                self._write(', ' + argname)
            self._write('):\n')
            self._write("        '''\n")
            self._write('        Overridable handler method for ' +
                        'when a %s message is successfully parsed.\n' %
                        msgtype)
            self._write("        '''\n")
            self._write('        return\n\n\n')

        # Emit serializer functions for module
        for msgtype in msgdict.keys():

            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]

            self._write('def serialize_' + msgtype +
                        '(' + ', '.join(self._getargnames(msgstuff)) + '):\n')
            self._write("    '''\n")
            self._write('    Serializes the contents of a message of type '
                        + msgtype + '.\n')
            self._write("    '''\n")
            self._write('    message_buffer = struct.pack(\'')
            for argtype in self._getargtypes(msgstuff):
                self._write(self.type2pack[argtype])
            self._write('\'')
            for argname in self._getargnames(msgstuff):
                self._write(', ' + argname)
            self._write(')\n\n    ')

            self._write('if sys.version[0] == \'2\':\n')
            self._write(self.indent*2 + 'msg = chr(len(message_buffer)) + '
                        'chr(%s) + str(message_buffer)\n' % msgid)
            self._write(self.indent*2 + ('return \'$M%c\' + msg + ' +
                        'chr(_CRC8(msg))\n\n') % ('>' if msgid < 200 else '<'))
            self._write(self.indent+'else:\n')
            self._write(self.indent*2 + ('msg = [len(message_buffer), %s] + ' +
                        'list(message_buffer)\n') % msgid)
            self._write(self.indent*2 + 'return bytes([ord(\'$\'), ' +
                        'ord(\'M\'), ord(\'<\')] + msg + [_CRC8(msg)])\n\n')

            if msgid < 200:

                self._write('def serialize_' + msgtype + '_Request():\n\n')
                self._write(self.indent + "'''\n")
                self._write(self.indent + 'Serializes a request for ' +
                            msgtype + ' data.\n')
                self._write(self.indent + "'''\n")
                self._write(self.indent + ('msg = \'$M<\' + chr(0) + '
                            'chr(%s) + chr(%s)\n') % (msgid, msgid))
                self._write(self.indent + 'return bytes(msg) ' +
                            'if sys.version[0] == \'2\' else ' +
                            'bytes(msg, \'utf-8\')\n\n')

    def _write(self, s):

        self.output.write(s)

# Java emitter ================================================================

class Java_Emitter(CompileableCodeEmitter):

    def __init__(self, msgdict):

        CompileableCodeEmitter.__init__(self, 'java', 'java')

        self.type2decl = {'byte': 'byte',
                          'short': 'short',
                          'float': 'float',
                          'int': 'int'}

        self.type2bb = {'byte': '',
                        'short': 'Short',
                        'float': 'Float',
                        'int': 'Int'}

        self.output = _openw('output/java/edu/wlu/cs/msppg/MyParser.java')

        # Write handler cases for incoming messages
        for msgtype in msgdict.keys():

            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]

            if msgid < 200:

                self._write(6*self.indent + 'case (byte)%d:\n' % msgid)
                self._write(8*self.indent + 'this.handle_%s(\n' % msgtype)

                argnames = self._getargnames(msgstuff)
                argtypes = self._getargtypes(msgstuff)

                nargs = len(argnames)

                offset = 0
                for k in range(nargs):
                    argtype = argtypes[k]
                    self._write(8*self.indent + 'bb.get%s(%d)' %
                                (self.type2bb[argtype], offset))
                    offset += self.type2size[argtype]
                    if k < nargs-1:
                        self._write(',\n')
                self._write(');\n')

                self._write(7*self.indent + 'break;\n\n')

        self._write(5*self.indent + '}\n' + 4*self.indent + '}\n' +
                    2*self.indent + '}\n' + self.indent + '}\n\n')

        for msgtype in msgdict.keys():

            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]

            argnames = self._getargnames(msgstuff)
            argtypes = self._getargtypes(msgstuff)

            # For messages from FC
            if msgid < 200:

                # Write serializer for requests
                self._write(self.indent +
                            'public byte [] serialize_%s_Request() {\n\n' %
                            msgtype)
                self._write('\n' + 2*self.indent +
                            'byte [] message = new byte[6];\n\n')
                self._write(2*self.indent + 'message[0] = 36;\n')
                self._write(2*self.indent + 'message[1] = 77;\n')
                self._write(2*self.indent + 'message[2] = 60;\n')
                self._write(2*self.indent + 'message[3] = 0;\n')
                self._write(2*self.indent + 'message[4] = (byte)%d;\n' %
                            msgid)
                self._write(2*self.indent + 'message[5] = (byte)%d;\n\n' %
                            msgid)
                self._write(2*self.indent + 'return message;\n')
                self._write(self.indent + '}\n\n')

                # Write handler for replies from flight controller
                self._write(self.indent + 'protected void handle_%s' %
                            msgtype)
                self._write_params(self.output, argtypes, argnames)
                self._write(' { \n')
                self._write(self.indent + '}\n\n')

        self._write('}\n')

    def _write(self, s):

        self.output.write(s)

# main ========================================================================


def main():

    # parse file name from command line
    parser = argparse.ArgumentParser()
    parser.add_argument('--filename', type=str, required=False,
                        default='messages.json',
                        help='Random seed for reproducibility')
    args = parser.parse_args()

    data = json.load(open(args.filename, 'r'))

    # takes the types of messages from the json file
    unicode_message_types = data.keys()

    # make a list of messages from the JSON file
    message_type_list = list()
    for key in unicode_message_types:
        message_type = json.dumps(key)
        clean_type = clean(message_type)
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
            argname = clean(clean(json.dumps(list(arg.keys()))))
            argtype = arg[list(arg.keys())[0]]
            if argname == 'ID':
                msgid = int(argtype)
            else:
                argtypes.append(argtype)
                argnames.append(argname)
            argument_lists.append(argnames)
        if msgid is None:
            error('Missing ID for message ' + msgtype)
        argument_types.append(argtypes)
        msgdict[msgtype] = (msgid, argnames, argtypes)

    # Emit Python
    Python_Emitter(msgdict)

    # Emite Java
    # Java_Emitter(msgdict)


if __name__ == '__main__':
    main()
