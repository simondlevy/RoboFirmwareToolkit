#!/usr/bin/python3
'''
Multiwii Serial Protocol Parser Generator

Generates serialtask.hpp based on messages.json

Copyright (C) Simon D. Levy 2021

MIT License
'''

from sys import exit
import os
import json
from pkg_resources import resource_string
import argparse


def clean(string):
    cleaned_string = string[1: len(string) - 1]
    return cleaned_string


def mkdir_if_missing(dirname):
    if not os.path.exists(dirname):
        os.mkdir(dirname)


def error(errmsg):
    print(errmsg)
    exit(1)


def _openw(fname):

    print('Creating file ' + fname)
    return open(fname, 'w')


class HPP_Emitter:

    type2decl = {'byte': 'uint8_t',
                 'short': 'int16_t',
                 'float': 'float',
                 'int': 'int32_t'}

    def __init__(self, msgdict):

        self.indent = '    '
        self.type2size = {'byte': 1, 'short': 2, 'float': 4, 'int': 4}
        self.type2decl = HPP_Emitter.type2decl

        # Open file for appending
        self.output = open('seritaltask.hpp', 'w')

        # Add dispatchMessage() method

        self.output.write(3*self.indent + 'void dispatchMessage(void)\n')
        self.output.write(3*self.indent + '{\n')
        self.output.write(4*self.indent + 'switch (_command) {\n\n')

        for msgtype in msgdict.keys():

            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]

            argnames = self._getargnames(msgstuff)
            argtypes = self._getargtypes(msgstuff)

            self.output.write(5*self.indent + ('case %s:\n' %
                                               msgdict[msgtype][0]))
            self.output.write(5*self.indent + '{\n')
            nargs = len(argnames)
            offset = 0
            for k in range(nargs):
                argname = argnames[k]
                argtype = argtypes[k]
                decl = self.type2decl[argtype]
                self.output.write(6*self.indent + decl + ' ' + argname +
                                  ' = 0;\n')
                if msgid >= 200:
                    fmt = 'memcpy(&%s,  &_inBuf[%d], sizeof(%s));\n\n'
                    self.output.write(6*self.indent +
                                      fmt % (argname, offset, decl))
                offset += self.type2size[argtype]
            self.output.write(6*self.indent + 'handle_%s%s(' %
                              (msgtype, '_Request' if msgid < 200 else ''))
            for k in range(nargs):
                self.output.write(argnames[k])
                if k < nargs-1:
                    self.output.write(', ')
            self.output.write(');\n')
            if msgid < 200:
                # XXX enforce uniform type for now
                argtype = argtypes[0].capitalize()
                self.output.write(6*self.indent + ('prepareToSend%ss(%d);\n' %
                                  (argtype, nargs)))
                for argname in argnames:
                    self.output.write(6*self.indent + ('send%s(%s);\n' %
                                      (argtype, argname)))
                self.output.write(6*self.indent + "serialize8(_checksum);\n")
            self.output.write(6*self.indent + '} break;\n\n')

        self.output.write(4*self.indent + '}\n')
        self.output.write(3*self.indent + '}\n\n')

        # Add virtual declarations for handler methods

        for msgtype in msgdict.keys():

            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]

            argnames = self._getargnames(msgstuff)
            argtypes = self._getargtypes(msgstuff)

            self.output.write(3*self.indent + 'virtual void handle_%s%s' %
                              (msgtype, '_Request' if msgid < 200 else ''))
            self._write_params(self.output, argtypes, argnames,
                               ampersand=('&' if msgid < 200 else ''))
            self.output.write('\n' + 3*self.indent + '{\n')
            for argname in argnames:
                self.output.write(4*self.indent + '(void)%s;\n' % argname)
            self.output.write(3*self.indent + '}\n\n')

        # Add message-serialization declarations to header

        self.output.write(self.indent*2 + 'public:\n\n')

        for msgtype in msgdict.keys():

            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]

            argnames = self._getargnames(msgstuff)
            argtypes = self._getargtypes(msgstuff)

            # Incoming messages
            if msgid < 200:

                # Write request method
                fmt = 'static uint8_t serialize_%s_Request(uint8_t bytes[])\n'
                self.output.write(3*self.indent + fmt % msgtype)
                self.output.write(3*self.indent + '{\n')
                self.output.write(4*self.indent + 'bytes[0] = 36;\n')
                self.output.write(4*self.indent + 'bytes[1] = 77;\n')
                self.output.write(4*self.indent + 'bytes[2] = %d;\n' %
                                  60 if msgid < 200 else 62)
                self.output.write(4*self.indent + 'bytes[3] = 0;\n')
                self.output.write(4*self.indent + 'bytes[4] = %d;\n' % msgid)
                self.output.write(4*self.indent + 'bytes[5] = %d;\n\n' %
                                  msgid)
                self.output.write(4*self.indent + 'return 6;\n')
                self.output.write(3*self.indent + '}\n\n')

            # Add parser method for serializing message
            self.output.write(3*self.indent + 'static uint8_t serialize_%s' %
                              msgtype)
            self._write_params(self.output, argtypes, argnames,
                               '(uint8_t bytes[], ')
            self.output.write('\n' + 3*self.indent + '{\n')
            msgsize = self._msgsize(argtypes)
            self.output.write(4*self.indent + 'bytes[0] = 36;\n')
            self.output.write(4*self.indent + 'bytes[1] = 77;\n')
            self.output.write(4*self.indent + 'bytes[2] = 62;\n')
            self.output.write(4*self.indent + 'bytes[3] = %d;\n' % msgsize)
            self.output.write(4*self.indent + 'bytes[4] = %d;\n\n' % msgid)
            nargs = len(argnames)
            offset = 5
            for k in range(nargs):
                argname = argnames[k]
                argtype = argtypes[k]
                decl = self.type2decl[argtype]
                self.output.write(4*self.indent +
                                  'memcpy(&bytes[%d], &%s, sizeof(%s));\n' %
                                  (offset, argname, decl))
                offset += self.type2size[argtype]
            self.output.write('\n')
            self.output.write(4*self.indent +
                              'bytes[%d] = CRC8(&bytes[3], %d);\n\n' %
                              (msgsize+5, msgsize+2))
            self.output.write(4*self.indent + 'return %d;\n' % (msgsize+6))
            self.output.write(3*self.indent + '}\n\n')

        self.output.write(self.indent + '}; // class MspParser\n\n')
        self.output.write('} // namespace hf\n')
        self.output.close()

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


# main ========================================================================


def main():

    # parse file name from command line
    parser = argparse.ArgumentParser()
    parser.add_argument('--infile', type=str, required=False,
                        default='messages.json',
                        help='Input file')
    parser.add_argument('--outfile', type=str, required=False,
                        default='serialtask.hpp',
                        help='Output file')
    parser.add_argument('--classname', type=str, required=False,
                        help='SerialTask')
    parser.add_argument('--namespace', type=str, required=False,
                        help='Namespace')
    args = parser.parse_args()

    data = json.load(open(args.infile, 'r'))

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

    # Emit firmware header
    HPP_Emitter(msgdict)


if __name__ == '__main__':
    main()
