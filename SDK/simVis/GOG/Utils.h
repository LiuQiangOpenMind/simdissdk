/* -*- mode: c++ -*- */
/****************************************************************************
 *****                                                                  *****
 *****                   Classification: UNCLASSIFIED                   *****
 *****                    Classified By:                                *****
 *****                    Declassify On:                                *****
 *****                                                                  *****
 ****************************************************************************
 *
 *
 * Developed by: Naval Research Laboratory, Tactical Electronic Warfare Div.
 *               EW Modeling & Simulation, Code 5773
 *               4555 Overlook Ave.
 *               Washington, D.C. 20375-5339
 *
 * License for source code at https://simdis.nrl.navy.mil/License.aspx
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 *
 */
#ifndef SIMVIS_GOG_UTILS_H
#define SIMVIS_GOG_UTILS_H

#include "simCore/Common/Common.h"
#include "simCore/Calc/Coordinate.h"
#include "simVis/GOG/GOGNode.h"
#include "osgEarth/Config"
#include "osgEarth/GeoData"
#include "osgEarth/Units"
#include "osgEarthSymbology/Geometry"
#include "osgEarthAnnotation/AnnotationData"
#include "osgEarthAnnotation/AnnotationNode"

namespace osgEarth { namespace Annotation { class LocalGeometryNode; } }

/**
 * Utilities used internally by the GOG parsers.
 */
namespace simVis { namespace GOG
{
  class ParserData;

  /** Utility class for the shapes */
  class SDKVIS_EXPORT Utils
  {
    public:
      /// enum describing Overlay line style
      enum LineStyle
      {
        LINE_SOLID,
        LINE_DASHED,
        LINE_DOTTED
      };

      /**
      * Apply local geometry offsets to the node as defined in the ParserData, including any position and orientation offsets
      * @param node  shape node the receive offsets
      * @param data  parsed offset data to apply to the shape node
      */
      static void applyLocalGeometryOffsets(osgEarth::Annotation::LocalGeometryNode& node, ParserData& data);

      /**
      * Determines if the specified shape's geometry can be serialized directly into Overlay format. This is dependent on how the shapes
      * are constructed from osg::Nodes. Things like lines and polygons have few points representing the vertices, which matches. However circles
      * and ellipses are made up of multiple points around the circumference, and these points will not easily translate into Overlay format
      */
      static bool canSerializeGeometry_(simVis::GOG::GogShape shape);

      /**
      * Get a vector of all the points in the Geometry. Handle the case where the geometry may be a MultiGeometry, for shapes like
      * line segs. Fills up the points param with all the point values, in standard osgEarth format, lon/lat/alt, units are deg/deg/meters
      */
      static void getGeometryPoints(const osgEarth::Symbology::Geometry* geometry, std::vector<osg::Vec3d>& points);

      /**
      * Returns the LineStyle based on the stipple value
      * @param stipple
      * @return the equivalent line style
      */
      static LineStyle getLineStyleFromStipple(unsigned short stipple);

      /**
      * Retrieve the osgEarth stipple value from the LineStyle
      * @param lineStyle
      * @return osgEarth stipple value
      */
      static unsigned short getStippleFromLineStyle(LineStyle lineStyle);

      /**
      * Decrypt the geometry object to determine if it is a MultiGeometry, then serialize the position information
      * from the osgEarth::Symbology::Geometry into a string in the standard GOG format
      * @param geometry to serialize
      * @param relativeShape  true if these are relative positions, false for absolute
      * @param gogOutputStream  ostream that holds the serialized position information
      */
      static void serializeShapeGeometry(const osgEarth::Symbology::Geometry* geometry, bool relativeShape, std::ostream& gogOutputStream);

      /**
      * Serialize the position information from the osgEarth::Symbology::Geometry into a string in the standard GOG format.
      * Applies the keyword 'xyz' if relative, 'lla otherwise
      * @param geometry to serialize
      * @param relativeShape  true if these are relative positions, false for absolute
      * @param gogOutputStream  ostream that holds the serialized position information
      */
      static void serializeGeometry(const osgEarth::Symbology::Geometry* geometry, bool relativeShape, std::ostream& gogOutputStream);

      /**
      * Serialize the osg color into a AGBR hex string
      * @param colorVec  osg color vector (RGBA)
      * @return string representation of hex AGBR color
      */
      static std::string serializeOsgColor(const osg::Vec4f& colorVec);

      /**
      * Serialize the line style, converts the enum into a string
      * @param lineStyle   enum
      * @return string representation of the linestyle enum
      */
      static std::string serializeLineStyle(LineStyle lineStyle);

      /**
       * True if the points in the geometry all have a zero Z value OR the
       * style calls for terrain-clamping. This is useful to know since "2D"
       * geometry will likely encounter Z-buffer issues, and therefore we
       * need to account for that.
       */
      static bool isGeometry2D(const osgEarth::Symbology::Geometry* geom);

      /**
       * If the geometry in this parser is "2D" (as determined by isGeometry2D)
       * update its rendering style to prevent it from z-fighting with the terrain.
       * You should call this after all the normal style and geometry parsing
       * has completed.
       */
      static void configureStyleForClipping(osgEarth::Symbology::Style& style);
  };

  /**
   * Current state of default units. This structure communicates to parsing elements
   * what Units are in effect when parsing coordinate and measurement data.
   * (utility structure - no export)
   */
  struct SDKVIS_EXPORT UnitsState
  {
    osgEarth::Units altitudeUnits_; ///< Altitude units
    osgEarth::Units rangeUnits_; ///< Range units
    osgEarth::Units timeUnits_; ///< Time units
    osgEarth::Units angleUnits_; ///< Angle units

    /**
     * Construct the units state
     */
    UnitsState();

    /**
     * Initializes the units state from a structured representation.
     * @param[in ] conf Structured data input
     */
    void parse(const osgEarth::Config& conf);

    /**
     * Initialized the units state from a GOG string.
     * @param[in ] s     Input string to parse
     * @param[in ] type  Units type (angular, linear, etc)
     * @param[out] units Parsed units
     */
    void parse(const std::string& s, osgEarth::Units::Type type, osgEarth::Units& units);
  };


  /**
   * "State" modifiers that "spill over" across GOG objects in the GOG file.
   *
   * In a GOG, certain state elements becomes active until they change, even
   * across different GOG objects. So we have to track the current state using
   * this object.
   */
  struct SDKVIS_EXPORT ModifierState
  {
    osgEarth::optional<std::string> lineColor_; ///< Line color
    osgEarth::optional<std::string> lineWidth_; ///< Line width
    osgEarth::optional<std::string> lineStyle_; ///< Line style
    osgEarth::optional<std::string> fillColor_; ///< File color
    osgEarth::optional<std::string> pointSize_; ///< Point size
    osgEarth::optional<std::string> altitudeMode_; ///< Altitude mode
    osgEarth::optional<std::string> altitudeUnits_; ///< Altitude units
    osgEarth::optional<std::string> rangeUnits_; ///< Range units
    osgEarth::optional<std::string> timeUnits_; ///< Time units
    osgEarth::optional<std::string> angleUnits_; ///< Angle units
    osgEarth::optional<std::string> verticalDatum_; ///< Vertical datum
    osgEarth::optional<std::string> priority_; ///< Label Priority

    /**
     * Stores the modifier state in a structured object.
     * @param[in ] conf Object in which to store the state
     */
    void apply(osgEarth::Config& conf);
  };


  /**
   * Data structure that persists which parsing GOG data.
   */
  class SDKVIS_EXPORT ParserData
  {
  public:
    /**
     * Initialize the parsing data from a structured object
     * @param[in ] conf    Input data (GOG stored in a Config)
     * @param[in ] context Shared parser information
     * @param[in ] shape   Type of shape being parsed
     */
    ParserData(const osgEarth::Config& conf, const GOGContext& context, GogShape shape);

    /** initialize the parser data */
    void init();

    /**
     * Reads geometry (coordinate sets) from a Config object. Use the
     * template parameter to pass in the type you want returned.
     * @param[in ] conf Config from which to read geometry
     */
    template<typename T>
    void parseGeometry(const osgEarth::Config& conf)
    {
      geom_ = new T();
      parsePoints(conf, units_, geom_.get(), geomIsLLA_);
    }

    /**
     * Reads a Geometry object.
     * @param[in ] parent Parent object from which to read coords
     * @param[in ] us     Unit state (to determine units of input coords)
     * @param[out] geom   Where to store the parsed points
     * @param[out] isLLA  Whether the output is lat/long/alt data
     */
    void parsePoints(const osgEarth::Config& parent, const UnitsState& us, osgEarth::Geometry* geom, bool& isLLA);

    /**
    * Accounts for the unique requirements of a line segment points
    * @param[in ] parent Parent object from which to read coords
    * @param[in ] us     Unit state (to determine units of input coords)
    * @param[out] geom   Where to store the parsed points
    * @param[out] isLLA  Whether the output is lat/long/alt data
    */
    void parseLineSegmentPoints(const osgEarth::Config& parent, const UnitsState& us, osgEarth::Geometry* geom, bool& isLLA);

    /**
     * Parses a string containing a angular coordinate value. Supports
     * decimal degrees, DDM, and DMS is various formats.
     */
    double parseAngle(const std::string& input, double fallback =0.0);

    /**
     * Reads optional offset and host-tracking properties.
     * @param[in ] parent Node from which to read data
     */
    void parseOffsetsAndTracking(const osgEarth::Config& parent);

    /**
     * Whether the current GOG has an absolute Map position.
     * @return True if so.
     */
    bool hasMapPosition() const;

    /**
     * Gets the absolute map position parsed for the current GOG
     * @return A map point (valid only is hasMapPosition()==true)
     */
    osgEarth::GeoPoint getMapPosition() const;

    /**
     * Gets the offset of the GOG from a local tangent plane (in meters)
     * @return ENU offset in meters
     */
    osg::Vec3d getLTPOffset() const;

    /**
     * Reads the parsed position into (absolute or relative) into the
     * output coordinate.
     */
    simCore::Coordinate getCoordinate(osgEarth::MapNode* mapNode, osgEarth::Style& style) const;

    /**
     * Whether the current object has absolute (map coordinate) geometry
     * (as opposed to LTP offset geometry)
     */
    bool hasAbsoluteGeometry() const;

    /**
     * Whether the current object has LTP-relative geometry (as opposed to
     * absolute map position geometry)
     */
    bool hasRelativeGeometry() const;

    /**
     * Whether the configuration of the geometry will result in its being
     * coincident with the terrain, a possible z-fighting condition.
     */
    bool geometryRequiresClipping() const;

    /**
     * Name of the current GOG.
     */
    const std::string& getName() const { return name_; }

    /**
     * Applies general properties parsed from the GOG input to an Annotation node.
     * @param[in ] node Annotation node to target
     */
    void applyToAnnotationNode(osg::Node* node);

    GOGContext                           context_; ///< Context
    osgEarth::Style                      style_; ///< Style
    osgEarth::optional<osg::Vec3d>       refPointLLA_; ///< Reference point in LLA
    osgEarth::optional<osg::Vec3d>       centerXYZ_; ///< Center point in XXZ
    osgEarth::optional<osg::Vec3d>       centerLLA_; ///< Center point in LLA
    osg::ref_ptr<osgEarth::Geometry>     geom_; ///< Geometry
    bool                                 geomIsLLA_; ///< If true than in LLA; if false in XYZ
    bool                                 geomIsRelative_; ///< If true than GOG is relative; if false GOG is absolute
    UnitsState                           units_; ///< Units for the GOG
    osgEarth::optional<osgEarth::GeoInterpolation> geoInterp_; ///< Interpolation
    osg::ref_ptr<const osgEarth::SpatialReference> srs_; ///< Spatial Reference
    std::string                          name_; ///< Name of the GOG
    unsigned int                         locatorComps_;  ///< Combination of heading, pitch and roll

    osgEarth::optional<osg::Vec3f>       scale_; ///< Scale of the GOG
    osgEarth::optional<osgEarth::Distance> localAltOffset_;  ///< Altitude offset of the GOG
    osgEarth::optional<osgEarth::Angle>  localHeadingOffset_; ///< Heading offset of the GOG
    osgEarth::optional<osgEarth::Angle>  localPitchOffset_; ///< Pitch offset of the GOG
    osgEarth::optional<osgEarth::Angle>  localRollOffset_; ///< Roll offset of the GOG
  };

} } // namespace simVis::GOG

#endif // SIMVIS_GOG_H
