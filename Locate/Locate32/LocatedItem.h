/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(LOCATEDITEM_H)
#define LOCATEDITEM_H

#if _MSC_VER >= 1000
#pragma once
#endif

class CLocater;

// For dwFlags
#define LITEM_FILETITLEOK			0x1
#define LITEM_TYPEOK				0x2
#define LITEM_ICONOK				0x4
#define LITEM_FILESIZEOK			0x8
#define LITEM_TIMEDATEOK			0x10
#define LITEM_ATTRIBOK				0x20
#define LITEM_PARENTICONOK			0x40
#define LITEM_EXTRASOK				0x100 // Only for refreshing
#define LITEM_ISDELETEDOK			0x200
#define LITEM_COULDCHANGE			LITEM_TYPEOK|LITEM_ICONOK|LITEM_FILESIZEOK|LITEM_TIMEDATEOK|LITEM_ATTRIBOK|LITEM_EXTRASOK|LITEM_ISDELETEDOK

#define LITEMATTRIB_HIDDEN			0x1
#define LITEMATTRIB_READONLY		0x2
#define LITEMATTRIB_ARCHIVE			0x4
#define LITEMATTRIB_SYSTEM			0x8
#define LITEMATTRIB_CUTTED			0x10
#define LITEMATTRIB_SYMLINK			0x20
#define LITEMATTRIB_JUNCTION		0x40
#define LITEMATTRIB_DIRECTORY		0x80

#define LITEMATTRIB_COMPRESSED		0x0200
#define LITEMATTRIB_ENCRYPTED		0x0400

#define LITEMATTRIB_DBATTRIBFLAG	0x00EF // CUTTED, COMPRESSED and EXCRYPTED are not from DB

class CLocatedItem 
{
public:
	CLocatedItem();
	~CLocatedItem();
	
	void SetFolder(const CLocater* pLocater);
	void SetFolderW(const CLocater* pLocater);
	void SetFile(const CLocater* pLocater);
	void SetFileW(const CLocater* pLocater);
	
private:
	
	void ClearData();

public:
	void ReFresh(CArray<DetailType>& aDetails,BOOL& bReDraw);
	void ReFresh(CArray<DetailType>& aDetails,int* pUpdated);
	

	void UpdateByDetail(DetailType nDetail);
	void UpdateFileTitle();
	void UpdateIcon();
	void UpdateParentIcon();
	void UpdateType();
	void UpdateFileSize();
	void UpdateFileTime();
	void UpdateFileSizeAndTime();
	void UpdateAttributes();
	void UpdateDimensions();
	void UpdateOwner();
	void UpdateShortFileName();
	void UpdateShortFilePath();
	void ComputeMD5sum(BOOL bForce=FALSE,BOOL bOnlyIfNotComputedYet=FALSE);
	void UpdateSummaryProperties();
	void UpdateDocSummaryProperties();
	void UpdateVersionInformation();
	void LoadThumbnail();
	void CheckIfDeleted();
	
	void SetToDeleted();
	
	BOOL ChangeName(CWnd* pWnd,LPCWSTR szNewName,int iLength=-1);
	BOOL ChangeParentName(CWnd* pWnd,LPCWSTR szNewName,int iLength=-1);

	DWORD GetFlags() const { return dwFlags; }
	void AddFlags(DWORD dwAdd) { dwFlags|=dwAdd; }
	void RemoveFlags(DWORD dwRemove) { dwFlags&=~dwRemove; }
	BOOL RemoveFlagsForChanged(); // Returing ok if changed
	
	BOOL ShouldUpdateByDetail(DetailType nDetail) const;
	BOOL ShouldUpdateFileTitle() const;
	BOOL ShouldUpdateType() const;
	BOOL ShouldUpdateIcon() const;
	BOOL ShouldUpdateFileSize() const;
	BOOL ShouldUpdateTimeAndDate() const;
	BOOL ShouldUpdateFileSizeOrDate() const;
	BOOL ShouldUpdateAttributes() const;
	BOOL ShouldUpdateParentIcon() const;
	BOOL ShouldUpdateParentIcon2() const;
	BOOL ShouldCheckIfDeleted() const;
	
	BOOL ShouldUpdateExtra(DetailType nDetail) const;

	
	// Accessors
	WORD GetDatabaseID() const { return wDatabaseID; }
	WORD GetRootID() const { return wRootID; }

	WORD GetAttributes() const { return wAttribs; }
	void AddAttribute(WORD wAttribute) { wAttribs|=wAttribute; }
	void RemoveAttribute(WORD wAttribute) { wAttribs&=~wAttribute; }

	BOOL IsDeleted() const { return iIcon==((CLocateApp*)GetApp())->m_nDelImage && dwFileSize==DWORD(-1) && wModifiedTime==WORD(-1) && wModifiedDate==WORD(-1); }
	BOOL IsFolder() const { return wAttribs&LITEMATTRIB_DIRECTORY; }
	BOOL IsHidden() const { return wAttribs&LITEMATTRIB_HIDDEN; }
	BOOL IsReadOnly() const { return wAttribs&LITEMATTRIB_READONLY; }
	BOOL IsSystem() const { return wAttribs&LITEMATTRIB_SYSTEM; }
	BOOL IsArchive() const { return wAttribs&LITEMATTRIB_ARCHIVE; }
	BOOL IsCutted() const { return wAttribs&LITEMATTRIB_CUTTED; }
	BOOL IsCompressed() const { return wAttribs&LITEMATTRIB_COMPRESSED; }
	BOOL IsEncrypted() const { return wAttribs&LITEMATTRIB_ENCRYPTED; }
	BOOL IsSymlink() const { return wAttribs&LITEMATTRIB_SYMLINK; }
	BOOL IsJunkction() const { return wAttribs&LITEMATTRIB_JUNCTION; }
	
	LPWSTR GetName() const { ASSERT_VALID(this); return szName; }
	DWORD GetNameLen() const { ASSERT_VALID(this); return bNameLength; }
	LPWSTR GetPath() const { ASSERT_VALID(this); szName[-1]=L'\\'; return szPath; }
	DWORD GetPathLen() const { ASSERT_VALID(this); return bNameLength+DWORD(szName-szPath); }
	LPWSTR GetParent() const { ASSERT_VALID(this); szName[-1]=L'\0'; return szPath; }
	CAutoPtrA<WCHAR> GetParentSafe() const { ASSERT_VALID(this); return alloccopy(szPath,DWORD(szName-szPath)); }
	LPWSTR GetFileTitle() const { ASSERT_VALID(this); return szFileTitle; }
	LPWSTR GetFileTitleSafe() const { ASSERT_VALID(this); if (szFileTitle==NULL) return szName; return szFileTitle; }
	LPWSTR GetType() const { ASSERT_VALID(this);  return szType; }
	
	LPWSTR GetDetailText(DetailType nDetailType) const;
	LPWSTR GetToolTipText() const;


	int GetIcon() const { return iIcon; }
	int GetParentIcon() const { return iParentIcon; }
	WORD GetModifiedDate() const { return wModifiedDate; }
	WORD GetModifiedTime() const { return wModifiedTime; }
	WORD GetCreatedDate() const { return wCreatedDate; }
	WORD GetCreatedTime() const { return wCreatedTime; }
	WORD GetAccessedDate() const { return wAccessedDate; }
	WORD GetAccessedTime() const { return wAccessedTime; }
	DWORD GetFileSizeLo() const { return dwFileSize; }
	DWORD GetFileSizeHi() const { return wFileSizeHi; }
	ULONGLONG GetFileSize() const { return ((ULONGLONG)wFileSizeHi)<<32|(ULONGLONG)dwFileSize; }
	LPWSTR GetExtension() const { return szName+bExtensionPos; }
	BYTE GetExtensionPos() const { return bExtensionPos; }
	DWORD GetExtensionLength() const { return bNameLength-bExtensionPos; }

	BOOL GetImageDimensions(SIZE& dim) const;
	int GetImageDimensionsProduct() const;
	int GetPages() const;
	HBITMAP GetThumbnail(SIZE& size) const;
	LPWSTR GetExtraText(DetailType nDetailType) const; 
	void ExtraSetUpdateWhenFileSizeChanged();
	void DeleteThumbnail();
    void DeleteAllExtraFields();
    BOOL IsItemShortcut() const;

	LPWSTR FormatAttributes() const;
	LPWSTR FormatOwner() const;
	LPWSTR FormatImageInformation() const;
	LPWSTR FormatPages() const;

private:
	static WORD GetAttributesFromSystemAttributes(DWORD dwSystemAttributess);
	static DWORD GetSystemAttributesFromAttributes(WORD wAttributess);
	


	LPWSTR szFileTitle;
	LPWSTR szName;
	mutable LPWSTR szPath;
	LPWSTR szType;
	int iIcon;
	int iParentIcon;

	DWORD dwFlags;
	WORD wModifiedDate;
	WORD wModifiedTime;
	WORD wCreatedDate;
	WORD wCreatedTime;
	WORD wAccessedDate;
	WORD wAccessedTime;
	DWORD dwFileSize;
	WORD wFileSizeHi;
	BYTE bExtensionPos;
	WORD wAttribs;
	BYTE bNameLength;

	WORD wDatabaseID;
	WORD wRootID;

	/* For extra field i.e. fields not obtained from db */
	struct ExtraInfo {
		ExtraInfo(DetailType nType);
		~ExtraInfo();
		
		BOOL ShouldUpdate() const;

		DetailType nType;
		
		struct ThumbnailData {
			ThumbnailData(HANDLE hMutex);
			~ThumbnailData();
			
			void CloseMutex();
			void WaitForLoading();

			HBITMAP hBitmap;
			CSize sThumbnailSize;
			HANDLE hLoadingMutex;
		};

		union {
			SIZE szImageDimension;
			WCHAR* szText;
			UINT nPages;
			ThumbnailData* pThumbnail;
		};
		BYTE bShouldUpdate;

		ExtraInfo* pNext;
	
	};
	ExtraInfo* pFirstExtraInfo;

	void DeleteExtraInfoField(DetailType nType);
	ExtraInfo* CreateExtraInfoField(DetailType nType);
	ExtraInfo* GetFieldForType(DetailType nType) const;
	

};



#endif
