/******************************************************************************
 * $Id$
 *
 * Name:     gdal_priv.h
 * Project:  GDAL Core
 * Purpose:  GDAL Core C++/Private declarations. 
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 1998, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log$
 * Revision 1.76  2006/11/28 12:56:53  mloskot
 * Excluded HFAAuxBuildOverviews function for Windows CE.
 *
 * Revision 1.75  2006/10/27 03:39:05  fwarmerdam
 * added GDALParseGMLCoverage
 *
 * Revision 1.74  2006/07/13 15:27:14  fwarmerdam
 * Implement ComputeStatistics method
 *
 * Revision 1.73  2006/05/24 22:25:33  fwarmerdam
 * split off DefaultCreateCopy() method.
 *
 * Revision 1.72  2006/04/13 03:16:01  fwarmerdam
 * keep track if an object is PAM enabled, bug 1135
 *
 * Revision 1.71  2006/02/07 19:07:07  fwarmerdam
 * applied some strategic improved outofmemory checking
 *
 * Revision 1.70  2005/11/17 22:02:32  fwarmerdam
 * avoid overwriting existing .aux file, overview filename now CPLString
 *
 * Revision 1.69  2005/10/13 01:19:57  fwarmerdam
 * moved GDALMultiDomainMetadata into GDALMajorObject
 *
 * Revision 1.68  2005/09/26 15:52:03  fwarmerdam
 * centralized .aux opening logic
 *
 * Revision 1.67  2005/09/23 20:52:21  fwarmerdam
 * added the GMO flags on GDALMajorObject
 *
 * Revision 1.66  2005/09/23 16:55:39  fwarmerdam
 * use CPLString instead of std::string
 *
 * Revision 1.65  2005/09/17 04:04:16  fwarmerdam
 * provide default implementation for RAT functions
 *
 * Revision 1.64  2005/09/17 03:46:37  fwarmerdam
 * added HFAAuxBuildOverviews
 *
 * Revision 1.63  2005/09/16 20:31:15  fwarmerdam
 * added bOvrIsAux, and RAT methods
 *
 * Revision 1.62  2005/07/25 21:24:28  ssoule
 * Changed GDALColorTable's "GDALColorEntry *paoEntries" to
 * "std::vector<GDALColorEntry> aoEntries".
 *
 * Revision 1.61  2005/07/25 19:52:43  ssoule
 * Changed GDALMajorObject's char *pszDescription to std::string sDescription.
 *
 * Revision 1.60  2005/07/11 21:08:17  fwarmerdam
 * Removed GetAge() method.
 *
 * Revision 1.59  2005/07/11 19:07:02  fwarmerdam
 * removed GDALRasterBlock nAge.
 *
 * Revision 1.58  2005/05/23 06:44:48  fwarmerdam
 * blockrefs now fetched locked
 *
 * Revision 1.57  2005/05/13 18:19:15  fwarmerdam
 * Added SetDefaultHistogram
 *
 * Revision 1.56  2005/05/10 15:30:28  fwarmerdam
 * export some overview functions
 *
 * Revision 1.55  2005/05/10 04:49:24  fwarmerdam
 * added getdefaulthistogram and GDALOvLevelAdjust
 *
 * Revision 1.54  2005/04/27 16:29:33  fwarmerdam
 * GetHistogram() made virtual.  Added methods for getting and setting
 * statistics.  Added SetUnitType method.
 *
 * Revision 1.53  2005/04/07 17:31:00  fwarmerdam
 * added some brief descriptions of classes
 *
 * Revision 1.52  2005/04/04 15:24:48  fwarmerdam
 * Most C entry points now CPL_STDCALL
 *
 * Revision 1.51  2005/02/17 22:16:12  fwarmerdam
 * changed to use two level block cache
 *
 * Revision 1.50  2005/01/15 16:09:37  fwarmerdam
 * added SetOffset, SetScale methods
 *
 * Revision 1.49  2005/01/04 21:14:01  fwarmerdam
 * added GDAL_FORCE_CACHING config variable
 */

#ifndef GDAL_PRIV_H_INCLUDED
#define GDAL_PRIV_H_INCLUDED

/* -------------------------------------------------------------------- */
/*      Predeclare various classes before pulling in gdal.h, the        */
/*      public declarations.                                            */
/* -------------------------------------------------------------------- */
class GDALMajorObject;
class GDALDataset;
class GDALRasterBand;
class GDALDriver;
class GDALRasterAttributeTable;

/* -------------------------------------------------------------------- */
/*      Pull in the public declarations.  This gets the C apis, and     */
/*      also various constants.  However, we will still get to          */
/*      provide the real class definitions for the GDAL classes.        */
/* -------------------------------------------------------------------- */

#include "gdal.h"
#include "gdal_frmts.h"
#include "cpl_vsi.h"
#include "cpl_conv.h"
#include "cpl_string.h"
#include "cpl_minixml.h"
#include <vector>

#define GMO_VALID                0x0001
#define GMO_IGNORE_UNIMPLEMENTED 0x0002
#define GMO_SUPPORT_MD           0x0004
#define GMO_SUPPORT_MDMD         0x0008
#define GMO_MD_DIRTY             0x0010
#define GMO_PAM_CLASS            0x0020

/************************************************************************/
/*                       GDALMultiDomainMetadata                        */
/************************************************************************/

class CPL_DLL GDALMultiDomainMetadata
{
private:
    char **papszDomainList;
    char ***papapszMetadataLists;

public:
    GDALMultiDomainMetadata();
    ~GDALMultiDomainMetadata();

    int         XMLInit( CPLXMLNode *psMetadata, int bMerge );
    CPLXMLNode  *Serialize();

    char      **GetDomainList() { return papszDomainList; }

    char      **GetMetadata( const char * pszDomain = "" );
    CPLErr      SetMetadata( char ** papszMetadata,
                             const char * pszDomain = "" );
    const char *GetMetadataItem( const char * pszName,
                                 const char * pszDomain = "" );
    CPLErr      SetMetadataItem( const char * pszName,
                                 const char * pszValue,
                                 const char * pszDomain = "" );

    void        Clear();
};

/* ******************************************************************** */
/*                           GDALMajorObject                            */
/*                                                                      */
/*      Base class providing metadata, description and other            */
/*      services shared by major objects.                               */
/* ******************************************************************** */

//! Object with metadata.

class CPL_DLL GDALMajorObject
{
  protected:
    int                 nFlags; // GMO_* flags. 
    CPLString           sDescription;
    GDALMultiDomainMetadata oMDMD;
    
  public:
                        GDALMajorObject();
    virtual            ~GDALMajorObject();

    int                 GetMOFlags();
    void                SetMOFlags(int nFlags);
                        
    virtual const char *GetDescription() const;
    virtual void        SetDescription( const char * );

    virtual char      **GetMetadata( const char * pszDomain = "" );
    virtual CPLErr      SetMetadata( char ** papszMetadata,
                                     const char * pszDomain = "" );
    virtual const char *GetMetadataItem( const char * pszName,
                                         const char * pszDomain = "" );
    virtual CPLErr      SetMetadataItem( const char * pszName,
                                         const char * pszValue,
                                         const char * pszDomain = "" );
};

/* ******************************************************************** */
/*                         GDALDefaultOverviews                         */
/* ******************************************************************** */
class CPL_DLL GDALDefaultOverviews
{
    GDALDataset *poDS;
    GDALDataset *poODS;
    
    CPLString  osOvrFilename;

    int        bOvrIsAux;
    
  public:
               GDALDefaultOverviews();
               ~GDALDefaultOverviews();

    void       Initialize( GDALDataset *poDS, const char *pszName = NULL, 
                           int bNameIsOVR = FALSE );
    int        IsInitialized() { return poDS != NULL; }

    int        GetOverviewCount(int);
    GDALRasterBand *GetOverview(int,int);

    CPLErr     BuildOverviews( const char * pszBasename,
                               const char * pszResampling, 
                               int nOverviews, int * panOverviewList,
                               int nBands, int * panBandList,
                               GDALProgressFunc pfnProgress,
                               void *pProgressData );
};

/* ******************************************************************** */
/*                             GDALDataset                              */
/* ******************************************************************** */

//! A set of associated raster bands, usually from one file.

class CPL_DLL GDALDataset : public GDALMajorObject
{
    friend GDALDatasetH CPL_STDCALL GDALOpen( const char *, GDALAccess);
    friend GDALDatasetH CPL_STDCALL GDALOpenShared( const char *, GDALAccess);
    friend class GDALDriver;

  protected:
    GDALDriver  *poDriver;
    GDALAccess  eAccess;
    
    // Stored raster information.
    int         nRasterXSize;
    int         nRasterYSize;
    int         nBands;
    GDALRasterBand **papoBands;

    int         bForceCachedIO;

    int         nRefCount;
    int         bShared;

                GDALDataset(void);
    void        RasterInitialize( int, int );
    void        SetBand( int, GDALRasterBand * );

    GDALDefaultOverviews oOvManager;
    
    virtual CPLErr IBuildOverviews( const char *, int, int *,
                                    int, int *, GDALProgressFunc, void * );
    
    virtual CPLErr IRasterIO( GDALRWFlag, int, int, int, int,
                              void *, int, int, GDALDataType,
                              int, int *, int, int, int );

    CPLErr BlockBasedRasterIO( GDALRWFlag, int, int, int, int,
                               void *, int, int, GDALDataType,
                               int, int *, int, int, int );
    void   BlockBasedFlushCache();

    friend class GDALRasterBand;
    
  public:
    virtual     ~GDALDataset();

    int         GetRasterXSize( void );
    int         GetRasterYSize( void );
    int         GetRasterCount( void );
    GDALRasterBand *GetRasterBand( int );

    virtual void FlushCache(void);

    virtual const char *GetProjectionRef(void);
    virtual CPLErr SetProjection( const char * );

    virtual CPLErr GetGeoTransform( double * );
    virtual CPLErr SetGeoTransform( double * );

    virtual CPLErr        AddBand( GDALDataType eType, 
                                   char **papszOptions=NULL );

    virtual void *GetInternalHandle( const char * );
    virtual GDALDriver *GetDriver(void);

    virtual int    GetGCPCount();
    virtual const char *GetGCPProjection();
    virtual const GDAL_GCP *GetGCPs();
    virtual CPLErr SetGCPs( int nGCPCount, const GDAL_GCP *pasGCPList,
                            const char *pszGCPProjection );

    virtual CPLErr AdviseRead( int nXOff, int nYOff, int nXSize, int nYSize,
                               int nBufXSize, int nBufYSize, 
                               GDALDataType eDT, 
                               int nBandCount, int *panBandList,
                               char **papszOptions );

    CPLErr      RasterIO( GDALRWFlag, int, int, int, int,
                          void *, int, int, GDALDataType,
                          int, int *, int, int, int );

    int           Reference();
    int           Dereference();
    GDALAccess    GetAccess() { return eAccess; }

    int           GetShared();
    void          MarkAsShared();

    static GDALDataset **GetOpenDatasets( int *pnDatasetCount );

    CPLErr BuildOverviews( const char *, int, int *,
                           int, int *, GDALProgressFunc, void * );
};

/* ******************************************************************** */
/*                           GDALRasterBlock                            */
/* ******************************************************************** */

/*! A cached raster block ... to be documented later. */

class CPL_DLL GDALRasterBlock
{
    GDALDataType        eType;
    
    int                 bDirty;
    int                 nLockCount;

    int                 nXOff;
    int                 nYOff;
       
    int                 nXSize;
    int                 nYSize;
    
    void                *pData;

    GDALRasterBand      *poBand;
    
    GDALRasterBlock     *poNext;
    GDALRasterBlock     *poPrevious;

  public:
                GDALRasterBlock( GDALRasterBand *, int, int );
    virtual     ~GDALRasterBlock();

    CPLErr      Internalize( void );    /* make copy of data */
    void        Touch( void );          /* update age */
    void        MarkDirty( void );      /* data has been modified since read */
    void        MarkClean( void );
    void        AddLock( void ) { nLockCount++; }
    void        DropLock( void ) { nLockCount--; }
    void        Detach();

    CPLErr      Write();

    GDALDataType GetDataType() { return eType; }
    int         GetXOff() { return nXOff; }
    int         GetYOff() { return nYOff; }
    int         GetXSize() { return nXSize; }
    int         GetYSize() { return nYSize; }
    int         GetDirty() { return bDirty; }
    int         GetLockCount() { return nLockCount; }

    void        *GetDataRef( void ) { return pData; }

    GDALRasterBand *GetBand() { return poBand; }

    static int  FlushCacheBlock();
    static void Verify();

    static int  SafeLockBlock( GDALRasterBlock ** );
};

/* ******************************************************************** */
/*                             GDALColorTable                           */
/* ******************************************************************** */

class CPL_DLL GDALColorTable
{
    GDALPaletteInterp eInterp;

    std::vector<GDALColorEntry> aoEntries;

public:
                GDALColorTable( GDALPaletteInterp = GPI_RGB );
                ~GDALColorTable();

    GDALColorTable *Clone() const;

    GDALPaletteInterp GetPaletteInterpretation() const;

    int           GetColorEntryCount() const;
    const GDALColorEntry *GetColorEntry( int ) const;
    int           GetColorEntryAsRGB( int, GDALColorEntry * ) const;
    void          SetColorEntry( int, const GDALColorEntry * );
};

/* ******************************************************************** */
/*                            GDALRasterBand                            */
/* ******************************************************************** */

//! A single raster band (or channel).

class CPL_DLL GDALRasterBand : public GDALMajorObject
{
  protected:
    GDALDataset *poDS;
    int         nBand; /* 1 based */

    int         nRasterXSize;
    int         nRasterYSize;
    
    GDALDataType eDataType;
    GDALAccess  eAccess;

    /* stuff related to blocking, and raster cache */
    int         nBlockXSize;
    int         nBlockYSize;
    int         nBlocksPerRow;
    int         nBlocksPerColumn;

    int         bSubBlockingActive;
    int         nSubBlocksPerRow;
    int         nSubBlocksPerColumn;
    GDALRasterBlock **papoBlocks;

    int         nBlockReads;
    int         bForceCachedIO;

    friend class GDALDataset;
    friend class GDALRasterBlock;

  protected:
    virtual CPLErr IReadBlock( int, int, void * ) = 0;
    virtual CPLErr IWriteBlock( int, int, void * );
    virtual CPLErr IRasterIO( GDALRWFlag, int, int, int, int,
                              void *, int, int, GDALDataType,
                              int, int );
    CPLErr         OverviewRasterIO( GDALRWFlag, int, int, int, int,
                                     void *, int, int, GDALDataType,
                                     int, int );

    int            InitBlockInfo();

    CPLErr         AdoptBlock( int, int, GDALRasterBlock * );
    GDALRasterBlock *TryGetLockedBlockRef( int nXBlockOff, int nYBlockYOff );

  public:
                GDALRasterBand();
                
    virtual     ~GDALRasterBand();

    int         GetXSize();
    int         GetYSize();
    int         GetBand();
    GDALDataset*GetDataset();

    GDALDataType GetRasterDataType( void );
    void        GetBlockSize( int *, int * );
    GDALAccess  GetAccess();
    
    CPLErr      RasterIO( GDALRWFlag, int, int, int, int,
                          void *, int, int, GDALDataType,
                          int, int );
    CPLErr      ReadBlock( int, int, void * );

    CPLErr      WriteBlock( int, int, void * );

    GDALRasterBlock *GetLockedBlockRef( int nXBlockOff, int nYBlockOff, 
                                        int bJustInitialize = FALSE );
    CPLErr      FlushBlock( int = -1, int = -1 );

    // New OpengIS CV_SampleDimension stuff.

    virtual CPLErr FlushCache();
    virtual char **GetCategoryNames();
    virtual double GetNoDataValue( int *pbSuccess = NULL );
    virtual double GetMinimum( int *pbSuccess = NULL );
    virtual double GetMaximum(int *pbSuccess = NULL );
    virtual double GetOffset( int *pbSuccess = NULL );
    virtual double GetScale( int *pbSuccess = NULL );
    virtual const char *GetUnitType();
    virtual GDALColorInterp GetColorInterpretation();
    virtual GDALColorTable *GetColorTable();
    virtual CPLErr Fill(double dfRealValue, double dfImaginaryValue = 0);

    virtual CPLErr SetCategoryNames( char ** );
    virtual CPLErr SetNoDataValue( double );
    virtual CPLErr SetColorTable( GDALColorTable * ); 
    virtual CPLErr SetColorInterpretation( GDALColorInterp );
    virtual CPLErr SetOffset( double );
    virtual CPLErr SetScale( double );
    virtual CPLErr SetUnitType( const char * );

    virtual CPLErr GetStatistics( int bApproxOK, int bForce,
                                  double *pdfMin, double *pdfMax, 
                                  double *pdfMean, double *padfStdDev );
    virtual CPLErr ComputeStatistics( int bApproxOK, 
                                      double *pdfMin, double *pdfMax, 
                                      double *pdfMean, double *padfStdDev,
                                      GDALProgressFunc, void *pProgressData );
    virtual CPLErr SetStatistics( double dfMin, double dfMax, 
                                  double dfMean, double dfStdDev );

    virtual int HasArbitraryOverviews();
    virtual int GetOverviewCount();
    virtual GDALRasterBand *GetOverview(int);
    virtual CPLErr BuildOverviews( const char *, int, int *,
                                   GDALProgressFunc, void * );

    virtual CPLErr AdviseRead( int nXOff, int nYOff, int nXSize, int nYSize,
                               int nBufXSize, int nBufYSize, 
                               GDALDataType eDT, char **papszOptions );

    virtual CPLErr  GetHistogram( double dfMin, double dfMax,
                          int nBuckets, int * panHistogram,
                          int bIncludeOutOfRange, int bApproxOK,
                          GDALProgressFunc, void *pProgressData );

    virtual CPLErr GetDefaultHistogram( double *pdfMin, double *pdfMax,
                                        int *pnBuckets, int ** ppanHistogram,
                                        int bForce,
                                        GDALProgressFunc, void *pProgressData);
    virtual CPLErr SetDefaultHistogram( double dfMin, double dfMax,
                                        int nBuckets, int *panHistogram );

    virtual const GDALRasterAttributeTable *GetDefaultRAT();
    virtual CPLErr SetDefaultRAT( const GDALRasterAttributeTable * );
};

/* ******************************************************************** */
/*                             GDALOpenInfo                             */
/*                                                                      */
/*      Structure of data about dataset for open functions.             */
/* ******************************************************************** */

class CPL_DLL GDALOpenInfo
{
  public:

                GDALOpenInfo( const char * pszFile, GDALAccess eAccessIn );
                ~GDALOpenInfo( void );
    
    char        *pszFilename;

    GDALAccess  eAccess;

    int         bStatOK;
    int         bIsDirectory;

    FILE        *fp;

    int         nHeaderBytes;
    GByte       *pabyHeader;

};

/* ******************************************************************** */
/*                              GDALDriver                              */
/* ******************************************************************** */


/**
 * \brief Format specific driver.
 *
 * An instance of this class is created for each supported format, and
 * manages information about the format.
 * 
 * This roughly corresponds to a file format, though some          
 * drivers may be gateways to many formats through a secondary     
 * multi-library.                                                  
 */

class CPL_DLL GDALDriver : public GDALMajorObject
{
  public:
                        GDALDriver();
                        ~GDALDriver();

/* -------------------------------------------------------------------- */
/*      Public C++ methods.                                             */
/* -------------------------------------------------------------------- */
    GDALDataset         *Create( const char * pszName,
                                 int nXSize, int nYSize, int nBands,
                                 GDALDataType eType, char ** papszOptions );

    CPLErr              Delete( const char * pszName );

    GDALDataset         *CreateCopy( const char *, GDALDataset *, 
                                     int, char **,
                                     GDALProgressFunc pfnProgress, 
                                     void * pProgressData );
    
    GDALDataset         *DefaultCreateCopy( const char *, GDALDataset *, 
                                            int, char **,
                                            GDALProgressFunc pfnProgress, 
                                            void * pProgressData );
    
/* -------------------------------------------------------------------- */
/*      The following are semiprivate, not intended to be accessed      */
/*      by anyone but the formats instantiating and populating the      */
/*      drivers.                                                        */
/* -------------------------------------------------------------------- */
    GDALDataset         *(*pfnOpen)( GDALOpenInfo * );

    GDALDataset         *(*pfnCreate)( const char * pszName,
                                       int nXSize, int nYSize, int nBands,
                                       GDALDataType eType,
                                       char ** papszOptions );

    CPLErr              (*pfnDelete)( const char * pszName );

    GDALDataset         *(*pfnCreateCopy)( const char *, GDALDataset *, 
                                           int, char **,
                                           GDALProgressFunc pfnProgress, 
                                           void * pProgressData );

    void                *pDriverData;

    void                (*pfnUnloadDriver)(GDALDriver *);
};

/* ******************************************************************** */
/*                          GDALDriverManager                           */
/* ******************************************************************** */

/**
 * Class for managing the registration of file format drivers.
 *
 * Use GetGDALDriverManager() to fetch the global singleton instance of
 * this class.
 */

class CPL_DLL GDALDriverManager : public GDALMajorObject
{
    int         nDrivers;
    GDALDriver  **papoDrivers;

    char        *pszHome;
    
 public:
                GDALDriverManager();
                ~GDALDriverManager();
                
    int         GetDriverCount( void );
    GDALDriver  *GetDriver( int );
    GDALDriver  *GetDriverByName( const char * );

    int         RegisterDriver( GDALDriver * );
    void        MoveDriver( GDALDriver *, int );
    void        DeregisterDriver( GDALDriver * );

    void        AutoLoadDrivers();
    void        AutoSkipDrivers();

    const char *GetHome();
    void        SetHome( const char * );
};

CPL_C_START
GDALDriverManager CPL_DLL * GetGDALDriverManager( void );
CPL_C_END

/* ==================================================================== */
/*      An assortment of overview related stuff.                        */
/* ==================================================================== */

CPL_C_START

#ifndef WIN32CE

CPLErr CPL_DLL
HFAAuxBuildOverviews( const char *pszOvrFilename, GDALDataset *poParentDS,
                      GDALDataset **ppoDS,
                      int nBands, int *panBandList,
                      int nNewOverviews, int *panNewOverviewList, 
                      const char *pszResampling, 
                      GDALProgressFunc pfnProgress, 
                      void *pProgressData );

#endif /* WIN32CE */

CPLErr CPL_DLL 
GTIFFBuildOverviews( const char * pszFilename,
                     int nBands, GDALRasterBand **papoBandList, 
                     int nOverviews, int * panOverviewList,
                     const char * pszResampling, 
                     GDALProgressFunc pfnProgress, void * pProgressData );

CPLErr CPL_DLL
GDALDefaultBuildOverviews( GDALDataset *hSrcDS, const char * pszBasename,
                           const char * pszResampling, 
                           int nOverviews, int * panOverviewList,
                           int nBands, int * panBandList,
                           GDALProgressFunc pfnProgress, void * pProgressData);
                           

CPLErr CPL_DLL 
GDALRegenerateOverviews( GDALRasterBand *, int, GDALRasterBand **,
                         const char *, GDALProgressFunc, void * );

int CPL_DLL GDALOvLevelAdjust( int nOvLevel, int nXSize );

GDALDataset CPL_DLL *
GDALFindAssociatedAuxFile( const char *pszBasefile, GDALAccess eAccess );

/* ==================================================================== */
/*      Misc functions.                                                 */
/* ==================================================================== */

CPLErr CPL_DLL GDALParseGMLCoverage( CPLXMLNode *psTree, 
                                     int *pnXSize, int *pnYSize,
                                     double *padfGeoTransform,
                                     char **ppszProjection );
                                  
CPL_C_END

#endif /* ndef GDAL_PRIV_H_INCLUDED */
