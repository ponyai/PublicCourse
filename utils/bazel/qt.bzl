def base_name(f):
  return f.split(".")[0]

def extension(f):
  return f.split(".")[1]

def cc_qt_library(name, srcs = [], hdrs = [], resources = [], **kwargs):
  """Compiles a QT library."""
  addition_hdrs = []
  addition_srcs = []
  for hdr in hdrs:
    if extension(hdr) == 'ui':
      ui_name = 'ui_' + base_name(hdr)
      native.genrule(
          name = ui_name,
          srcs = [hdr],
          outs = ["%s.h" % ui_name],
          cmd = "/usr/lib/x86_64-linux-gnu/qt5/bin/uic $(location %s) -o $@" % hdr,
      )
      addition_hdrs.append(":%s" % ui_name)
    elif extension(hdr) == "qrc":
      qrc_name = "qrc_" + base_name(hdr)
      native.genrule(
          name = qrc_name,
          srcs = [hdr] + resources,
          outs = ["%s.cc" % qrc_name],
          # NOTE(team): "*.qrc" must be in the same directory with resource root.
          # If you want to use resource only in *.ui files, please make alwayslink=1,
          # otherwise, the resource will not be linked.
          cmd = "/usr/lib/x86_64-linux-gnu/qt5/bin/rcc $(location %s) -o $@" % hdr
      )
      addition_srcs.append(":%s" % qrc_name)
    else:
      moc_name = base_name(hdr) + '_moc'
      native.genrule(
          name = moc_name,
          srcs = [hdr],
          outs = ["%s.cc" % moc_name],
          # NOTE(yiming): On some machines (e.g., mine), /usr/bin/moc is actually moc of qt4.
          # Therefore, provide the absolute path here to make sure Qt5 moc is used.
          cmd = "/usr/lib/x86_64-linux-gnu/qt5/bin/moc $(location %s) -o $@ -f'%s' -nw"  \
                  % (hdr, '%s/%s' % (native.package_name(), hdr)),
      )
      addition_srcs.append(":%s" % moc_name)

  native.cc_library(
      name = name,
      srcs = srcs + addition_srcs,
      hdrs = hdrs + addition_hdrs,
      **kwargs
  )
