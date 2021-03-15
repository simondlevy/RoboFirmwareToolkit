#!/usr/bin/python3
'''
Multiwii Serial Protocol Parser Generator

Generates serialtask.hpp based on messages.json

Copyright (C) Simon D. Levy 2021

MIT License
'''

import json
import argparse

type2decl = {'byte': 'uint8_t',
             'short': 'int16_t',
             'float': 'float',
             'int': 'int32_t'}

type2size = {'byte': 1, 'short': 2, 'float': 4, 'int': 4}


def clean(string):
    cleaned_string = string[1: len(string) - 1]
    return cleaned_string


def getargs(message):

    return [(argname, argtype) for (argname, argtype) in
            zip(message[1], message[2]) if argname.lower() != 'comment']


def getargnames(message):

    return [argname for (argname, _) in getargs(message)]


def getargtypes(message):

    return [argtype for (_, argtype) in getargs(message)]


def paysize(argtypes):

    return sum([type2size[atype] for atype in argtypes])


def write_params(outfile, argtypes, argnames, prefix='(', ampersand=''):

    outfile.write(prefix)
    for argtype, argname in zip(argtypes, argnames):
        outfile.write(type2decl[argtype] + ' ' + ampersand + ' ' + argname)
        if argname != argnames[-1]:
            outfile.write(', ')
    outfile.write(')')


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

    # Open output file
    output = open(args.outfile, 'w')

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
    if args.namespace is not None:
        output.write('namespace %s {\n\n' % args.namespace)

    # Add classname
    output.write('class %s {\n\n' % args.classname)
    
    # Add stubbed declarations for handler methods

    for msgtype in msgdict.keys():

        msgstuff = msgdict[msgtype]
        msgid = msgstuff[0]

        argnames = getargnames(msgstuff)
        argtypes = getargtypes(msgstuff)

        output.write('        private: void handle_%s%s' %
                     (msgtype, '_Request' if msgid < 200 else ''))
        write_params(output, argtypes, argnames,
                     ampersand=('&' if msgid < 200 else ''))
        output.write('\n        {\n        }\n\n')

    # Add dispatchMessage() method

    output.write('        protected: void dispatchMessage(void) override\n')
    output.write('        {\n')
    output.write('            switch (_command) {\n\n')

    for msgtype in msgdict.keys():

        msgstuff = msgdict[msgtype]
        msgid = msgstuff[0]

        argnames = getargnames(msgstuff)
        argtypes = getargtypes(msgstuff)

        output.write('                case %s: {\n' % msgdict[msgtype][0])
        nargs = len(argnames)
        offset = 0
        for k in range(nargs):
            argname = argnames[k]
            argtype = argtypes[k]
            decl = type2decl[argtype]
            output.write('                    ' + decl + ' ' + argname + ' = 0;\n')
            if msgid >= 200:
                fmt = 'memcpy(&%s,  &_inBuf[%d], sizeof(%s));\n\n'
                output.write(' '*20 + fmt % (argname, offset, decl))
            offset += type2size[argtype]
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

    output.write('    }; // class %s\n\n' % args.classname)
    if args.namespace is not None:
        output.write('} // namespace hf\n')
    output.close()


if __name__ == '__main__':
    main()
