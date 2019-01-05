#!/bin/python

from waflib import *
import os, sys

top = '.'
out = 'build'

g_cflags = ["-Wall", "-Wextra", "-std=c++17"]
def btype_cflags(ctx):
	return {
		"DEBUG"   : g_cflags + ["-Og", "-ggdb3", "-march=core2", "-mtune=native"],
		"NATIVE"  : g_cflags + ["-Ofast", "-march=native", "-mtune=native"],
		"RELEASE" : g_cflags + ["-O3", "-march=core2", "-mtune=generic"],
	}.get(ctx.env.BUILD_TYPE, g_cflags)

def options(opt):
	opt.load("g++")
	opt.add_option('--build_type', dest='build_type', type="string", default='RELEASE', action='store', help="DEBUG, NATIVE, RELEASE")

def configure(ctx):
	ctx.load("g++")
	ctx.check(features='c cprogram', lib='pthread', uselib_store='PTHREAD')
	btup = ctx.options.build_type.upper()
	if btup in ["DEBUG", "NATIVE", "RELEASE"]:
		Logs.pprint("PINK", "Setting up environment for known build type: " + btup)
		ctx.env.BUILD_TYPE = btup
		ctx.env.CXXFLAGS = btype_cflags(ctx)
		Logs.pprint("PINK", "CXXFLAGS: " + ' '.join(ctx.env.CXXFLAGS))
		if btup == "DEBUG":
			ctx.define("DEBUG", 1)
	else:
		Logs.error("UNKNOWN BUILD TYPE: " + btup)
		
def build(bld):
	
	bld_install_files = bld.path.ant_glob('src/asterales/*.hh')
	bld.install_files('${PREFIX}/include/asterales', bld_install_files)
	
	bld_files = bld.path.ant_glob('src/*.cc')
	asterales = bld (
		features = "cxx cxxshlib",
		target = 'asterales',
		source = bld_files,
		uselib = ['PTHREAD'],
	)
	
	b2t = bld (
		features = "cxx cxxprogram",
		target = 'aeon2json',
		source = 'tools/b2t.cc',
		use = ['asterales'],
		includes = [os.path.join(top, 'src')],
	)
	
	t2b = bld (
		features = "cxx cxxprogram",
		target = 'json2aeon',
		source = 'tools/t2b.cc',
		use = ['asterales'],
		includes = [os.path.join(top, 'src')],
	)
	
	tests = bld(
		features = "cxx cxxprogram",
		target = 'asterales_tests',
		source = bld.path.ant_glob('tests/*.cc'),
		use = ['asterales'],
		includes = [os.path.join(top, 'src')],
	)
