/**
 * SECTION:element-djimoonlightshimsink
 * @title: djimoonlightshimsink
 *
 * Send a video stream to a DJI goggles running dji-moonlight-shim.
 *
 * ## Example launch lines
 * |[
 * gst-launch-1.0 videotestsrc ! "video/x-raw,width=1920,height=1080,framerate=30/1" ! x264enc ! h264parse ! djimoonlightshimsink
 * ]| Generate a test video and display it in the DJI goggles.
 * |[
 * gst-launch-1.0 v4l2src ! videoconvert ! x264enc speed-preset=ultrafast tune=zerolatency ! h264parse ! djimoonlightshimsink
 * ]| View Video4Linux2 webcam in the DJI goggles.
 */

#include "gstdjimoonlightshimsink.h"
#include "dji_usb.h"
#include "config.h"

GST_DEBUG_CATEGORY_STATIC(gst_dji_moonlight_shim_sink_debug);
#define GST_CAT_DEFAULT gst_dji_moonlight_shim_sink_debug

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE("sink",
                                                                   GST_PAD_SINK,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS("video/x-h264,profile=high,stream-format=byte-stream,alignment=nal"));

G_DEFINE_TYPE(GstDjiMoonlightShimSink, gst_dji_moonlight_shim_sink, GST_TYPE_BASE_SINK);

/* GstBaseSink vmethod implementations */

static gboolean gst_dji_moonlight_shim_sink_start(GstBaseSink *sink)
{
  GST_TRACE("START");
  return dji_usb_setup();
}

static gboolean gst_dji_moonlight_shim_sink_stop(GstBaseSink *sink)
{
  GST_TRACE("START");
  dji_usb_cleanup();
  return TRUE;
}

static gboolean gst_dji_moonlight_shim_sink_set_caps(GstBaseSink *sink, GstCaps *caps)
{
  GST_TRACE("START");
  if (((GstDjiMoonlightShimSink *)sink)->first_connect)
    ((GstDjiMoonlightShimSink *)sink)->first_connect = FALSE;
  else
    dji_usb_reset();

  if (gst_caps_get_size(caps) > 1)
  {
    GST_ERROR("oh no too many structs");
    return FALSE;
  }
  GstStructure *caps_struct = gst_caps_get_structure(caps, 0);

  gint fps_numerator, fps_denominator, framerate;
  gint width, height;

  if (!gst_structure_get_int(caps_struct, "width", &width))
  {
    GST_ERROR("Width is missing in set_caps");
    return FALSE;
  }
  if (!gst_structure_get_int(caps_struct, "height", &height))
  {
    GST_ERROR("Height is missing in set_caps");
    return FALSE;
  }
  if (!gst_structure_get_fraction(caps_struct, "framerate", &fps_numerator, &fps_denominator))
  {
    GST_ERROR("Framerate is missing in set_caps");
    return FALSE;
  }
  framerate = fps_numerator / (double)fps_denominator + 0.5;

  return dji_usb_connect(width, height, framerate);
}

static gboolean gst_dji_moonlight_shim_sink_render_internal(GstBuffer **buffer, guint idx, gpointer user_data)
{
  GST_TRACE("START");
  GstMapInfo map;
  if (!gst_buffer_map(*buffer, &map, GST_MAP_READ))
  {
    GST_ERROR("Failed to map buffer");
    return FALSE;
  }

  if (!dji_usb_transmit(map.data, map.size))
  {
    GST_ERROR("Failed to send buffer over usb");
    return FALSE;
  }

  return TRUE;
}

static GstFlowReturn gst_dji_moonlight_shim_sink_render(GstBaseSink *sink, GstBuffer *buffer)
{
  GST_TRACE("START");

  if (!gst_dji_moonlight_shim_sink_render_internal(&buffer, 0, NULL))
  {
    GST_ERROR("Failed to send buffer");
    return GST_FLOW_ERROR;
  }

  return GST_FLOW_OK;
}

static GstFlowReturn gst_dji_moonlight_shim_sink_render_list(GstBaseSink *sink, GstBufferList *list)
{
  GST_TRACE("START");

  if (!gst_buffer_list_foreach(list, gst_dji_moonlight_shim_sink_render_internal, NULL))
  {
    GST_ERROR("Failed to send buffer");
    return GST_FLOW_ERROR;
  }

  return GST_FLOW_OK;
}

/* GObject vmethod implementations */

static void gst_dji_moonlight_shim_sink_class_init(GstDjiMoonlightShimSinkClass *klass)
{
  GstElementClass *gstelement_class = (GstElementClass *)klass;
  GstBaseSinkClass *gstbasesink_class = GST_BASE_SINK_CLASS(klass);

  gst_element_class_add_static_pad_template(gstelement_class, &sinktemplate);

  gst_element_class_set_static_metadata(gstelement_class,
                                        "Dji Moonlight Shim Sink",
                                        "Sink/Video", "Send a video stream to a DJI goggles running dji-moonlight-shim",
                                        "Niklas Digakis <niklas dot digakis at w-hs dot de>");

  gstbasesink_class->start = GST_DEBUG_FUNCPTR(gst_dji_moonlight_shim_sink_start);
  gstbasesink_class->stop = GST_DEBUG_FUNCPTR(gst_dji_moonlight_shim_sink_stop);
  gstbasesink_class->render = GST_DEBUG_FUNCPTR(gst_dji_moonlight_shim_sink_render);
  gstbasesink_class->render_list = GST_DEBUG_FUNCPTR(gst_dji_moonlight_shim_sink_render_list);
  gstbasesink_class->set_caps = GST_DEBUG_FUNCPTR(gst_dji_moonlight_shim_sink_set_caps);
}

static void gst_dji_moonlight_shim_sink_init(GstDjiMoonlightShimSink *sink)
{
  sink->first_connect = TRUE;
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean dji_moonlight_shim_sink_init(GstPlugin *plugin)
{
  GST_DEBUG_CATEGORY_INIT(gst_dji_moonlight_shim_sink_debug, "djimoonlightshimsink", 0, "djimoonlightshimsink");
  return gst_element_register(plugin, "djimoonlightshimsink", GST_RANK_NONE, gst_dji_moonlight_shim_sink_get_type());
}

/* gstreamer looks for this structure to register videosplitters
 */
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  djimoonlightshim,
                  "GStreamer dji-moonlight-shim elemets",
                  dji_moonlight_shim_sink_init,
                  PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
