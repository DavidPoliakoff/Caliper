#-------------------------------------------------------------------------------
# Sample makefile for building the code samples. Read inline comments for
# documentation.
#
# Eli Bendersky (eliben@gmail.com)
# This code is in the public domain
#-------------------------------------------------------------------------------

# The following variables will likely need to be customized, depending on where
# and how you built LLVM & Clang. They can be overridden by setting them on the
# make command line: "make VARNAME=VALUE", etc.

# LLVM_SRC_PATH is the path to the root of the checked out source code. This
# directory should contain the configure script, the include/ and lib/
# directories of LLVM, Clang in tools/clang/, etc.
#
# Alternatively, if you're building vs. a binary distribution of LLVM
# (downloaded from llvm.org), then LLVM_SRC_PATH can point to the main untarred
# directory of the binary download (the directory that has bin/, lib/, include/
# and other directories inside).
# See the build_vs_released_binary.sh script for an example.
LLVM_SRC_PATH := /usr/global/tools/clang/chaos_5_x86_64_ib/clang-cuda-beta-2017-03-29/

# LLVM_BUILD_PATH is the directory in which you built LLVM - where you ran
# configure or cmake.
# For linking vs. a binary build of LLVM, point to the main untarred directory.
# LLVM_BIN_PATH is the directory where binaries are placed by the LLVM build
# process. It should contain the tools like opt, llc and clang. The default
# reflects a release build with CMake and Ninja. binary build of LLVM, point it
# to the bin/ directory.
LLVM_BUILD_PATH := $(LLVM_SRC_PATH)
LLVM_BIN_PATH 	:= $(LLVM_BUILD_PATH)/rawbin/

$(info -----------------------------------------------)
$(info Using LLVM_SRC_PATH = $(LLVM_SRC_PATH))
$(info Using LLVM_BUILD_PATH = $(LLVM_BUILD_PATH))
$(info Using LLVM_BIN_PATH = $(LLVM_BIN_PATH))
$(info -----------------------------------------------)

# CXX has to be a fairly modern C++ compiler that supports C++11. gcc 4.8 and
# higher or Clang 3.2 and higher are recommended. Best of all, if you build LLVM
# from sources, use the same compiler you built LLVM with.
# Note: starting with release 3.7, llvm-config will inject flags that gcc may
# not support (for example '-Wcovered-switch-default'). If you run into this
# problem, build with CXX set to a modern clang++ binary instead of g++.
CXX := clang++
CXXFLAGS := -fno-rtti -O0 -g -std=c++1z -w
PLUGIN_CXXFLAGS := -fpic

LLVM_CXXFLAGS := `$(LLVM_BIN_PATH)/llvm-config --cxxflags` -std=c++1z -w -g
LLVM_LDFLAGS := `$(LLVM_BIN_PATH)/llvm-config --ldflags --libs --system-libs`

# Plugins shouldn't link LLVM and Clang libs statically, because they are
# already linked into the main executable (opt or clang). LLVM doesn't like its
# libs to be linked more than once because it uses globals for configuration
# and plugin registration, and these trample over each other.
LLVM_LDFLAGS_NOLIBS := `$(LLVM_BIN_PATH)/llvm-config --ldflags` -std=c++1z -w -g
PLUGIN_LDFLAGS := -shared

# These are required when compiling vs. a source distribution of Clang. For
# binary distributions llvm-config --cxxflags gives the right path.
CLANG_INCLUDES := \
	-I$(LLVM_SRC_PATH)/tools/clang/include \
	-I$(LLVM_BUILD_PATH)/tools/clang/include

# List of Clang libraries to link. The proper -L will be provided by the
# call to llvm-config
# Note that I'm using -Wl,--{start|end}-group around the Clang libs; this is
# because there are circular dependencies that make the correct order difficult
# to specify and maintain. The linker group options make the linking somewhat
# slower, but IMHO they're still perfectly fine for tools that link with Clang.
CLANG_LIBS := \
	-Wl,--start-group \
	-lclangAST \
	-lclangASTMatchers \
	-lclangIndex \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSerialization \
	-lclangToolingCore \
	-lclangTooling \
	-lclangFormat \
	-Wl,--end-group

# Internal paths in this project: where to find sources, and where to put
# build artifacts.
SRC_LLVM_DIR := src_llvm
SRC_CLANG_DIR := src_clang
BUILDDIR := build

.PHONY: all
default: plugins
all: emit_build_config \
	$(BUILDDIR)/bb_toposort_sccs \
	$(BUILDDIR)/simple_module_pass \
	$(BUILDDIR)/simple_bb_pass \
	$(BUILDDIR)/analyze_geps \
	$(BUILDDIR)/hello_pass.so \
	$(BUILDDIR)/replace_threadidx_with_call \
	$(BUILDDIR)/access_debug_metadata \
	$(BUILDDIR)/clang-check \
	$(BUILDDIR)/rewritersample \
	$(BUILDDIR)/matchers_rewriter \
	$(BUILDDIR)/tooling_sample \
	$(BUILDDIR)/plugin_print_funcnames.so

dogs: \
	$(BUILDDIR)/plugin_print_funcnames.so

#plugins: $(BUILDDIR)/plugin_print_funcnames.so \
#		 $(BUILDDIR)/plugin_raja_checker.so

plugins: make_builddir \
		 $(BUILDDIR)/plugin_all_checkers.so \
		 $(BUILDDIR)/plugin_this_checker.so \
		 $(BUILDDIR)/plugin_managed_array.so \
		 $(BUILDDIR)/plugin_struct_member_checker.so \
		 $(BUILDDIR)/plugin_inside_use_checker.so \
		 $(BUILDDIR)/plugin_outside_use_checker.so 

#$(BUILDDIR)/plugin_use_checker.so
repro: plugins \
		$(BUILDDIR)/simple_repro.so

#plugins: $(BUILDDIR)/plugin_print_funcnames.so \
#		 $(BUILDDIR)/plugin_caliper_instrumenter.so \
#		 $(BUILDDIR)/plugin_raja_cuda_checker.so

cali: make_builddir \
	$(BUILDDIR)/caliper_instrumenter

.PHONY: test
test: emit_build_config
	python3 test/all_tests.py

.PHONY: emit_build_config
emit_build_config: make_builddir
	@echo $(LLVM_BIN_PATH) > $(BUILDDIR)/_build_config

.PHONY: make_builddir
make_builddir:
	@test -d $(BUILDDIR) || mkdir $(BUILDDIR)

$(BUILDDIR)/simple_bb_pass: $(SRC_LLVM_DIR)/simple_bb_pass.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/analyze_geps: $(SRC_LLVM_DIR)/analyze_geps.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/jit_orc_run: $(SRC_LLVM_DIR)/jit_orc_run.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/replace_threadidx_with_call: $(SRC_LLVM_DIR)/replace_threadidx_with_call.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/simple_module_pass: $(SRC_LLVM_DIR)/simple_module_pass.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/access_debug_metadata: $(SRC_LLVM_DIR)/access_debug_metadata.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/bb_toposort_sccs: $(SRC_LLVM_DIR)/bb_toposort_sccs.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/hello_pass.so: $(SRC_LLVM_DIR)/hello_pass.cpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) \
		$^ $(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@

$(BUILDDIR)/clang-check: $(SRC_CLANG_DIR)/ClangCheck.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/caliper_instrumenter: $(SRC_CLANG_DIR)/caliper_instrumenter.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/tooling_sample: $(SRC_CLANG_DIR)/tooling_sample.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/matchers_replacements: $(SRC_CLANG_DIR)/matchers_replacements.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/matchers_rewriter: $(SRC_CLANG_DIR)/matchers_rewriter.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/plugin_print_funcnames.so: $(SRC_CLANG_DIR)/plugin_print_funcnames.cpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@

$(BUILDDIR)/plugin_caliper_instrumenter.so: $(SRC_CLANG_DIR)/caliper_instrumenter.cpp $(SRC_CLANG_DIR)/infrastructure/llnl_static_analysis_utilities.h
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/caliper_instrumenter.cpp \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@

$(BUILDDIR)/plugin_all_checkers.so: $(SRC_CLANG_DIR)/raja_full_check.cpp $(SRC_CLANG_DIR)/infrastructure/llnl_static_analysis_utilities.h $(SRC_CLANG_DIR)/infrastructure/plugin_templates/RajaVisitor.hpp $(SRC_CLANG_DIR)/infrastructure/plugin_templates/Consumer.hpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/raja_full_check.cpp \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@
$(BUILDDIR)/plugin_this_checker.so: $(SRC_CLANG_DIR)/this_checker.cpp $(SRC_CLANG_DIR)/infrastructure/llnl_static_analysis_utilities.h $(SRC_CLANG_DIR)/infrastructure/plugin_templates/RajaVisitor.hpp $(SRC_CLANG_DIR)/infrastructure/plugin_templates/Consumer.hpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/this_checker.cpp \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@
$(BUILDDIR)/plugin_managed_array.so: $(SRC_CLANG_DIR)/managed_array_checker.cpp $(SRC_CLANG_DIR)/infrastructure/llnl_static_analysis_utilities.h $(SRC_CLANG_DIR)/infrastructure/plugin_templates/RajaVisitor.hpp $(SRC_CLANG_DIR)/infrastructure/plugin_templates/Consumer.hpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/managed_array_checker.cpp \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@
$(BUILDDIR)/plugin_struct_member_checker.so: $(SRC_CLANG_DIR)/struct_member_checker.cpp $(SRC_CLANG_DIR)/infrastructure/llnl_static_analysis_utilities.h $(SRC_CLANG_DIR)/infrastructure/plugin_templates/RajaVisitor.hpp $(SRC_CLANG_DIR)/infrastructure/plugin_templates/Consumer.hpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/struct_member_checker.cpp \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@
$(BUILDDIR)/plugin_inside_use_checker.so: $(SRC_CLANG_DIR)/inside_use_checker.cpp $(SRC_CLANG_DIR)/infrastructure/llnl_static_analysis_utilities.h $(SRC_CLANG_DIR)/infrastructure/plugin_templates/RajaVisitor.hpp $(SRC_CLANG_DIR)/infrastructure/plugin_templates/Consumer.hpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/inside_use_checker.cpp \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@
$(BUILDDIR)/plugin_outside_use_checker.so: $(SRC_CLANG_DIR)/outside_use_checker.cpp $(SRC_CLANG_DIR)/infrastructure/llnl_static_analysis_utilities.h $(SRC_CLANG_DIR)/infrastructure/plugin_templates/RajaVisitor.hpp $(SRC_CLANG_DIR)/infrastructure/plugin_templates/Consumer.hpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/outside_use_checker.cpp \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@

$(BUILDDIR)/plugin_use_checker.so: $(SRC_CLANG_DIR)/use_checker.cpp $(SRC_CLANG_DIR)/infrastructure/llnl_static_analysis_utilities.h $(SRC_CLANG_DIR)/infrastructure/plugin_templates/RajaVisitor.hpp $(SRC_CLANG_DIR)/infrastructure/plugin_templates/Consumer.hpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/use_checker.cpp \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@

rewritetest: $(BUILDDIR)/plugin_rewriter.so

$(BUILDDIR)/plugin_rewriter.so: $(SRC_CLANG_DIR)/rewriter.cpp $(SRC_CLANG_DIR)/infrastructure/llnl_static_analysis_utilities.h $(SRC_CLANG_DIR)/infrastructure/plugin_templates/RajaVisitor.hpp $(SRC_CLANG_DIR)/infrastructure/plugin_templates/Consumer.hpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/rewriter.cpp \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@

$(BUILDDIR)/simple_repro.so: $(SRC_CLANG_DIR)/simple_reproducer_plugin.cpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/simple_reproducer_plugin.cpp \
		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@
# Experimental tools - use at your own peril.
#
.PHONY: experimental_tools
experimental_tools: make_builddir \
	emit_build_config \
	$(BUILDDIR)/loop_info \
	$(BUILDDIR)/build_llvm_ir \
	$(BUILDDIR)/remove-cstr-calls \
	$(BUILDDIR)/toplevel_decls \
	$(BUILDDIR)/try_matcher

$(BUILDDIR)/loop_info: $(SRC_LLVM_DIR)/experimental/loop_info.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/build_llvm_ir: $(SRC_LLVM_DIR)/experimental/build_llvm_ir.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $^ $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/remove-cstr-calls: $(SRC_CLANG_DIR)/experimental/RemoveCStrCalls.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

clang-move: $(BUILDDIR)/tools/clang-move $(BUILDDIR)/tools/clang-move.so

#$(BUILDDIR)/tools/clang-move: $(BUILDDIR)/tools/clang-move.so $(SRC_CLANG_DIR)/../tools/clang-move/tool/ClangMoveMain.cpp
#	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) -I$(SRC_CLANG_DIR)/../tools/clang-move $(SRC_CLANG_DIR)/../tools/clang-move/ClangMove.cpp $^ \
#		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

#$(BUILDDIR)/tools/clang-move.so: make_builddir
#	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $(SRC_CLANG_DIR)/../tools/clang-move/ClangMove.cpp \
#		$(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@
		

$(BUILDDIR)/toplevel_decls: $(SRC_CLANG_DIR)/experimental/toplevel_decls.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

$(BUILDDIR)/try_matcher: $(SRC_CLANG_DIR)/experimental/try_matcher.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/* *.dot test/*.pyc test/__pycache__
