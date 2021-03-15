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


class Emitter:

    type2decl = {'byte': 'uint8_t',
                 'short': 'int16_t',
                 'float': 'float',
                 'int': 'int32_t'}

    def __init__(self, msgdict, filename, classname, namespace):

        indent = '    '
        self.type2size = {'byte': 1, 'short': 2, 'float': 4, 'int': 4}
        self.type2decl = Emitter.type2decl

        # Open file for appending
        output = open(filename, 'w')

        # Add dispatchMessage() method

        output.write(3*indent + 'void dispatchMessage(void)\n')
        output.write(3*indent + '{\n')
        output.write(4*indent + 'switch (_command) {\n\n')

        for msgtype in msgdict.keys():

            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]

            argnames = self._getargnames(msgstuff)
            argtypes = self._getargtypes(msgstuff)

            output.write(5*indent + ('case %s:\n' %
                                               msgdict[msgtype][0]))
            output.write(5*indent + '{\n')
            nargs = len(argnames)
            offset = 0
            for k in range(nargs):
                argname = argnames[k]
                argtype = argtypes[k]
                decl = self.type2decl[argtype]
                output.write(6*indent + decl + ' ' + argname +
                                  ' = 0;\n')
                if msgid >= 200:
                    fmt = 'memcpy(&%s,  &_inBuf[%d], sizeof(%s));\n\n'
                    output.write(6*indent +
                                      fmt % (argname, offset, decl))
                offset += self.type2size[argtype]
            output.write(6*indent + 'handle_%s%s(' %
                              (msgtype, '_Request' if msgid < 200 else ''))
            for k in range(nargs):
                output.write(argnames[k])
                if k < nargs-1:
                    output.write(', ')
            output.write(');\n')
            if msgid < 200:
                # XXX enforce uniform type for now
                argtype = argtypes[0].capitalize()
                output.write(6*indent + ('prepareToSend%ss(%d);\n' %
                                  (argtype, nargs)))
                for argname in argnames:
                    output.write(6*indent + ('send%s(%s);\n' %
                                      (argtype, argname)))
                output.write(6*indent + "serialize8(_checksum);\n")
            output.write(6*indent + '} break;\n\n')

        output.write(4*indent + '}\n')
        output.write(3*indent + '}\n\n')

        # Add virtual declarations for handler methods

        for msgtype in msgdict.keys():

            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]

            argnames = self._getargnames(msgstuff)
            argtypes = self._getargtypes(msgstuff)

            output.write(3*indent + 'virtual void handle_%s%s' %
                              (msgtype, '_Request' if msgid < 200 else ''))
            self._write_params(output, argtypes, argnames,
                               ampersand=('&' if msgid < 200 else ''))
            output.write('\n' + 3*indent + '{\n')
            for argname in argnames:
                output.write(4*indent + '(void)%s;\n' % argname)
            output.write(3*indent + '}\n\n')

        # Add message-serialization declarations to header

        output.write(indent*2 + 'public:\n\n')

        for msgtype in msgdict.keys():

            msgstuff = msgdict[msgtype]
            msgid = msgstuff[0]

            argnames = self._getargnames(msgstuff)
            argtypes = self._getargtypes(msgstuff)

            # Incoming messages
            if msgid < 200:

                # Write request method
                fmt = 'static uint8_t serialize_%s_Request(uint8_t bytes[])\n'
                output.write(3*indent + fmt % msgtype)
                output.write(3*indent + '{\n')
                output.write(4*indent + 'bytes[0] = 36;\n')
                output.write(4*indent + 'bytes[1] = 77;\n')
                output.write(4*indent + 'bytes[2] = %d;\n' %
                                  60 if msgid < 200 else 62)
                output.write(4*indent + 'bytes[3] = 0;\n')
                output.write(4*indent + 'bytes[4] = %d;\n' % msgid)
                output.write(4*indent + 'bytes[5] = %d;\n\n' %
                                  msgid)
                output.write(4*indent + 'return 6;\n')
                output.write(3*indent + '}\n\n')

            # Add parser method for serializing message
            output.write(3*indent + 'static uint8_t serialize_%s' %
                              msgtype)
            self._write_params(output, argtypes, argnames,
                               '(uint8_t bytes[], ')
            output.write('\n' + 3*indent + '{\n')
            msgsize = self._msgsize(argtypes)
            output.write(4*indent + 'bytes[0] = 36;\n')
            output.write(4*indent + 'bytes[1] = 77;\n')
            output.write(4*indent + 'bytes[2] = 62;\n')
            output.write(4*indent + 'bytes[3] = %d;\n' % msgsize)
            output.write(4*indent + 'bytes[4] = %d;\n\n' % msgid)
            nargs = len(argnames)
            offset = 5
            for k in range(nargs):
                argname = argnames[k]
                argtype = argtypes[k]
                decl = self.type2decl[argtype]
                output.write(4*indent +
                                  'memcpy(&bytes[%d], &%s, sizeof(%s));\n' %
                                  (offset, argname, decl))
                offset += self.type2size[argtype]
            output.write('\n')
            output.write(4*indent +
                              'bytes[%d] = CRC8(&bytes[3], %d);\n\n' %
                              (msgsize+5, msgsize+2))
            output.write(4*indent + 'return %d;\n' % (msgsize+6))
            output.write(3*indent + '}\n\n')

        output.write(indent + '}; // class %s\n\n' % classname)
        output.write('} // namespace hf\n')
        output.close()

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
                        default='SerialTask',
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
            print('Missing ID for message ' + msgtype)
            exit(1)
        argument_types.append(argtypes)
        msgdict[msgtype] = (msgid, argnames, argtypes)

    # Emit firmware header
    Emitter(msgdict, args.outfile, args.classname, args.namespace)


if __name__ == '__main__':
    main()
