#ifndef GST_DJI_MOONLIGHT_SHIM_SINK_H
#define GST_DJI_MOONLIGHT_SHIM_SINK_H

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>

G_BEGIN_DECLS

typedef struct _GstDjiMoonlightShimSink GstDjiMoonlightShimSink;
typedef struct _GstDjiMoonlightShimSinkClass GstDjiMoonlightShimSinkClass;

struct _GstDjiMoonlightShimSink
{
  GstBaseSink parent;

  gboolean first_connect;
};

struct _GstDjiMoonlightShimSinkClass
{
  GstBaseSinkClass parent_class;
};

G_GNUC_INTERNAL GType gst_dji_moonlight_shim_sink_get_type(void);

G_END_DECLS

#endif
