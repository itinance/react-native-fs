using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace RNFS
{
    public struct DiskStatus
    {
        public ulong free;
        public ulong total;
    }

    public class DiskUtil
    {
        // Pinvoke for API function
        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetDiskFreeSpaceEx(string lpDirectoryName,
        out ulong lpFreeBytesAvailable,
        out ulong lpTotalNumberOfBytes,
        out ulong lpTotalNumberOfFreeBytes);

        public static bool DriveFreeBytes(string folderName, out DiskStatus status)
        {
            if (string.IsNullOrEmpty(folderName))
            {
                throw new ArgumentNullException("folderName");
            }

            if (!folderName.EndsWith("\\"))
            {
                folderName += '\\';
            }

            ulong dummy;
            if (GetDiskFreeSpaceEx(folderName, out status.free, out status.total, out dummy))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
