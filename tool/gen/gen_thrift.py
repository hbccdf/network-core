#!/usr/bin/python
'''
    only for generating for c++ code
'''

import os, sys, time, functools, re, shutil, traceback

_generator = "thrift-0.10.0.exe"

_thrift_dir_name = "gen-cpp/"
_generate_dir_name = "gen_files"

_config_info = {}
_proto_dic = {}

_args = (
    #'--gen csharp',
    '--gen cpp',
    #'--gen cpp:moveable_types',
    #'--gen js:node'
    )

def _remove_underline(name):
    n = name.find("_")
    if n >= 0:
        return name[:n] + name[n+1:]
    else:
        return name

class _thrift:
    def __init__(self):
        self.name=""
        self.key=""
        self.space=""
        self.dir=""
        self.gene_space=""
        self.proto_id_namespace=""
        self.dic = {}

    def file_exist(self):
        if os.path.exists(self.name):
            return True
        return False

    def gen_thrift(self):
        for arg in _args:
            command = "%s %s %s" %(_generator, arg, self.name)
            #print command
            os.system(command)

    def get_gen_file_name(self):
        file_list=[]
        if len(self.key) > 0:
            pattern = re.compile(r'(\w+)_(\w+)(\W+)=(\W+)(\d+)', re.DOTALL)
            pattern_for_protoid = re.compile(r'enum(\W+)%s' % self.key, re.DOTALL)

            file = open(self.name, 'r')
            is_matched_protoid = False
            is_matched_lbrace = False
            is_matched_rbrace = False
            for line in file:
                if is_matched_protoid == False:
                    matched = pattern_for_protoid.search(line)
                    if matched:
                        s = matched.group()
                        self.proto_id_namespace = s[s.index(self.key):].strip()
                        is_matched_protoid = True

                if is_matched_protoid == True and is_matched_lbrace == False:
                    if line.find('{') != -1:
                        is_matched_lbrace = True
                        # print '---->2'

                if is_matched_protoid == True and is_matched_lbrace == True and is_matched_rbrace == False:
                    matched = pattern.search(line)
                    if matched:
                        s = matched.group()
                        s.strip()
                        s = s[:s.index('=')].rstrip()
                        #n = s.index('_')
                        #file_list.append(s[:n] + s[n + 1:])
                        file_list.append(s)

                    if line.find('}') != -1:
                        is_matched_rbrace = True
                        # print '---->3'
            file.close()
        return file_list

    def find_struct(self, struct_name):
        pattern_struct = re.compile(r'struct(\W+)%s' % struct_name, re.DOTALL)

        struct_matched = False
        file = open(self.name, 'r')
        for line in file:
            struct_matched = pattern_struct.search(line)
            if struct_matched :
                break

        file.close()
        return struct_matched

    def get_dic(self):
        for name in self.get_gen_file_name():
            struct_name = _remove_underline(name)
            finded = self.find_struct(struct_name)
            self.dic[name] = finded
            if not finded:
                print struct_name, "not finded"

        return self.dic

class _base:
    include_list=["<network/gameserver/game_server.hpp>"]
    left_brace='{'
    rigth_brace="}"
    base_class_name="Proto"

    def __init__(self, key_name, thrift, finded_struct):
        self.key_name = key_name
        self.new_name = _remove_underline(key_name)
        self.base_name = self.key_name
        self.finded_struct = finded_struct
        
        if not finded_struct:
            if self.key_name in _proto_dic:
                self.finded_struct = True
                self.base_name = _proto_dic[self.key_name]
                print self.key_name, "find struct in _proto_dic, name is", self.base_name
                
        self.class_name = "%s%s" % (self.new_name, _config_info["class_suffix"])
        n = self.base_name.find("_")
        if n >= 0:
            self.ref_inst_name = self.base_name[:n].lower() + self.base_name[n+1:]
        else:
            self.ref_inst_name = self.base_name[:2].lower() + self.base_name[2:]
            
        self.base_name = _remove_underline(self.base_name)

        self.namespace = "namespace %s" % thrift.space
        self.proto_id_namespace = thrift.proto_id_namespace

class _base_hpp(_base):
    def __init__(self, key_name, thrift, finded_struct):
        _base.__init__(self, key_name, thrift, finded_struct)
        self.gen_file_name = "%s_%s.hpp" % (self.new_name, _config_info["hpp_suffix"])
        self.thrift = thrift
        

    def get_file_name(self):
        return self.gen_file_name

    def get_content(self):
        content = "#pragma once\n\n"
        for include in self.include_list:
            content += "#include %s\n" % include
        content += "\n %s \n %s\n" % (self.namespace, self.left_brace)
        content += "    using namespace cytx::gameserver;\n"
        content += "    using namespace cytx;\n"
        content += self.get_base_content()
        content += "}\n"

        return content

    def get_base_content(self):
        content =  "    class %s : public %s\n    %s\n" %(self.class_name, self.base_class_name, self.left_brace)
        content += "        using this_t = %s;\n" % self.class_name
        content += "        using base_t = %s;\n" % self.base_class_name
        content += "    public:\n"
        content += "        %s()\n" % self.class_name
        content += "            : base_t(ProtoId()) {}\n"
        content += "        %s(const this_t& rhs)\n" % self.class_name
        content += "            : base_t(rhs)\n"
        if self.finded_struct:
            content += "            , %s(rhs.%s)\n" % (self.ref_inst_name, self.ref_inst_name)
        content += "        {}\n"
        if not self.finded_struct:
            content += "        msg_ptr pack() const\n"
            content += "        {\n"
            content += "            return pack_msg();\n"
            content += "        }\n"
            content += "        void pack(stream_t& gos) const\n"
            content += "        {\n"
            content += "        }\n"
            content += "        void unpack(msg_ptr& msgp) override\n"
            content += "        {\n"
            content += "        }\n"
        else:
            content += "        msg_ptr pack() const\n"
            content += "        {\n"
            content += "            return pack_msg(%s);\n" % self.ref_inst_name
            content += "        }\n"
            content += "        void pack(stream_t& gos) const\n"
            content += "        {\n"
            content += "            pack_msg(gos, %s);\n" % self.ref_inst_name
            content += "        }\n"
            content += "        void unpack(msg_ptr& msgp) override\n"
            content += "        {\n"
            content += "            %s = unpack_msg<%s>(msgp);\n" % (self.ref_inst_name, self.base_name)
            content += "        }\n"

        content += "\n    public:\n"
        content += "        static uint32_t ProtoId()\n"
        content += "        {\n"
        content += "            return %s::%s::%s;\n" % (self.thrift.gene_space, self.proto_id_namespace, self.key_name)
        content += "        }\n\n"

        if self.finded_struct:
            content += "    public:\n"
            content += "        %s %s;\n" % (self.base_name, self.ref_inst_name)

        content += "    };\n"

        return content

class _full_hpp(_base):
    def __init__(self, key_name, thrift):
        _base.__init__(self, key_name, thrift, True)
        self.gen_file_name = "%s_%ss.hpp" % (key_name, _config_info["hpp_suffix"])
        self.hpps = []
        self.thrift = thrift

    def get_file_name(self):
        return self.gen_file_name

    def add_hpp(self, hpp_file):
        self.hpps.append(hpp_file)

    def get_content(self):
        content = "#pragma once\n"
        for include in self.include_list:
            content += "#include %s\n" % include

        content += "#include \"%s_types.h\"\n" % self.thrift.dir
        content += "\n %s\n %s\n" % (self.namespace, self.left_brace)
        content += "    using namespace cytx::gameserver;\n"
        content += "    using namespace cytx;\n\n"
        content += "    using game_server_t = cytx::gameserver::game_server;\n\n"

        for hpp_file in self.hpps:
            content += hpp_file.get_base_content()
            content += "\n"

        content += "}\n"

        return content

class _cpp(_base):
    def __init__(self, key_name, thrift):
        _base.__init__(self, key_name, thrift, True)
        self.action_class_name = "%s%s" % (self.new_name, _config_info["cpp_suffix"])
        self.gen_file_name = "%s.cpp" % self.action_class_name
        self.thrift = thrift

    def get_file_name(self):
        return self.gen_file_name

    def get_content(self):
        
        content = "#include \"%s/all_%ss.hpp\"\n\n" % (_config_info["proto_dir"], _config_info["hpp_suffix"])
        content += "%s\n{\n" % self.namespace
        content += "    class %s : public %s\n" % (self.action_class_name, self.class_name)
        content += "        using this_t = %s;\n" % self.action_class_name
        content += "        using base_t = %s;\n" % self.class_name
        content += "        %s(const this_t& rhs)\n" % self.action_class_name
        content += "            : base_t(rhs)\n"
        content += "        {}\n"
        content += "    public:\n"
        content += "        proto_ptr_t clone() override\n"
        content += "        {\n"
        content += "            return std::make_shared<this_t>(*this);\n"
        content += "        }\n"
        content += "        void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server) override;\n"
        content += "    };\n\n"
        
        content += "    REGISTER_PROTOCOL(%s);\n\n" % self.action_class_name
        content += "    void %s::process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)\n" % self.action_class_name
        content += "    {\n\n"
        content += "    }\n"
        content += "}\n"

        return content

class _proto():
    def __init__(self, key_name):
        self.gen_file_name = key_name

    def get_content(self, list_ ):
        content = ""
        for file_name in list_:
            content += "#include \"%s\"\n" % file_name

        return content

    def create(self, list_):
        content = self.get_content(list_)
        file = open("%s/%s" % (_generate_dir_name, self.gen_file_name), "w")
        file.write(content)
        file.close()


class _g_factory:
    def create(self, type, file_name, thrift, is_full_hpp, finded_struct):
        file_suffix = _config_info["cpp_suffix"]
        if type == "hpp":
            full_name = "%s/%s%s.hpp" % (_generate_dir_name, _remove_underline(file_name), file_suffix)
            inst = _base_hpp(file_name, thrift, finded_struct)
        elif type == "cpp":
            full_name = "%s/%s%s.cpp" % (_generate_dir_name, _remove_underline(file_name), file_suffix)
            inst = _cpp(file_name, thrift)

        if not is_full_hpp:
            content = inst.get_content()
            file = open(full_name, "w")
            file.write(content)
            file.close()
        return inst

def create_dir(path):
    is_dir = os.path.isdir(path)
    if is_dir:
        shutil.rmtree(path)
    os.mkdir(path)

def move_file_to_dir(list, save_dir):
    create_dir(save_dir)
    for fname in list:
        file = os.path.join(os.path.abspath("."), _thrift_dir_name + fname)
        print "move", file, "->", save_dir
        shutil.move(file, save_dir)
        
def copytree(src, dst, symlinks=False):
    names = os.listdir(src)
    if not os.path.isdir(dst):
        os.makedirs(dst)
          
    errors = []
    for name in names:
        srcname = os.path.join(src, name)
        dstname = os.path.join(dst, name)
        try:
            if symlinks and os.path.islink(srcname):
                linkto = os.readlink(srcname)
                os.symlink(linkto, dstname)
            elif os.path.isdir(srcname):
                copytree(srcname, dstname, symlinks)
            else:
                if os.path.isdir(dstname):
                    os.rmdir(dstname)
                elif os.path.isfile(dstname):
                    os.remove(dstname)
                shutil.copy2(srcname, dstname)
            # XXX What about devices, sockets etc.?
        except (IOError, os.error) as why:
            errors.append((srcname, dstname, str(why)))
        # catch the Error from the recursive copytree so that we can
        # continue with other files
        except OSError as err:
            errors.extend(err.args[0])
    try:
        shutil.copystat(src, dst)
    except WindowsError:
        # can't copy file access times on Windows
        pass
    except OSError as why:
        errors.extend((src, dst, str(why)))
    if errors:
        raise Error(errors)

def copy_dir(src_dir, dst_dir):
    try:
        exist = os.path.isdir(src_dir)
        if exist:
            copytree(src_dir, dst_dir)
        else:
            print src_dir, ' not exist.'

    except:
        traceback.print_exc()
        # Write log file
        # f=open("log.txt",'a')
        # traceback.print_exc(file=f)
        # f.flush()
        # f.close()

############################################################################

_list_thrift = []

f1 = _thrift()
f1.name = "message.thrift"
f1.key = "MessageId"
f1.space = "CytxGame"
f1.gene_space = ""
f1.dir = "message"
_list_thrift.append(f1)

f2 = _thrift()
f2.name = "struct.thrift"
f2.key = ""
f2.space = ""
f2.gene_space = ""
f2.dir = "struct"
_list_thrift.append(f2)

_config_info["proto_dir"] = "proto"
_config_info["hpp_suffix"] = "action"
_config_info["cpp_suffix"] = "_Action"
_config_info["class_suffix"] = "_Msg"

copy_dst_dir = "../../example/common/proto"

############################################################################

if __name__ == "__main__":
    list_common_total = []
    copy_files = []
    for t in _list_thrift:
        if t.file_exist() == False:
            print "err:", t.name, "file not exist"
            break

        t.gen_thrift()

        copy_files.append("%s%s_constants.h" % (_thrift_dir_name, t.dir))
        copy_files.append("%s%s_constants.cpp" % (_thrift_dir_name, t.dir))
        copy_files.append("%s%s_types.h" % (_thrift_dir_name, t.dir))
        copy_files.append("%s%s_types.cpp" % (_thrift_dir_name, t.dir))

        gen_files = t.get_gen_file_name()
        if len(gen_files) == 0:
            continue

        gen_dic = t.get_dic()
        hpp_list = []
        cpp_list = []

        create_dir(_generate_dir_name)

        full_hpp_file = _full_hpp(t.dir, t)
        list_common_total.append(full_hpp_file.get_file_name())

        #print content
        for name in gen_files:
            gf = _g_factory()
            finded = gen_dic[name]
            hpp_file = gf.create("hpp", name, t, True, finded)
            hpp_list.append(hpp_file.get_file_name())
            full_hpp_file.add_hpp(hpp_file)

            cpp_file = gf.create("cpp", name, t, False, finded)
            cpp_list.append(cpp_file.get_file_name())

        full_name = "%s%s" % (_thrift_dir_name, full_hpp_file.get_file_name())
        copy_files.append(full_name)

        content = full_hpp_file.get_content()
        file = open(full_name, "w")
        file.write(content)
        file.close()

        
    hpp_suffix = _config_info["hpp_suffix"]
    
    print  hpp_suffix, "files", list_common_total

    
    
    content = "#pragma once\n"
    for wrap in list_common_total:
        content += "#include \"%s\"\n" % wrap

    full_name = "%s%ss_common.hpp" % (_thrift_dir_name, hpp_suffix)
    file = open(full_name, "w")
    file.write(content)
    file.close()

    #copy to dst dir
    src_dir = os.path.join(os.path.abspath("."), _thrift_dir_name)
    dst_dir = os.path.join(os.path.abspath("."), copy_dst_dir)
    print dst_dir
    copy_dir(src_dir, dst_dir)

    full_name = "%s/custom_%ss.hpp" % (dst_dir, hpp_suffix)
    if not os.path.exists(full_name):
        content = "#pragma once\n"
        file = open(full_name, "w")
        file.write(content)
        file.close()

    full_name = "%s/all_%ss.hpp" % (dst_dir, hpp_suffix)
    if not os.path.exists(full_name):
        content = "#pragma once\n"
        content += "#include \"%ss_common.hpp\"\n" % hpp_suffix
        content += "#include \"custom_%ss.hpp\"\n" % hpp_suffix
        file = open(full_name, "w")
        file.write(content)
        file.close()

os.system("pause")