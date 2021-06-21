/**
 * @file ascii.h
 * @author Stefan Gloor
 *
 */

#define TEX_SEPARATOR   "&"
#define TEX_NEWLINE	"\\\\"
#define TEX_HLINE	"\\hline"

/**
 * @brief Creates the latex table header
 * @param[out] f pointer to file to write
 * @param[in] number of headings
 * @param[in] number of wraps
 * @param[in] character string that is used to align, can be "l", "c" or "r".
 * @param[in] flag if tex output should be standalone
 * @note closeTableHeader() must be called to end table
 */ 
void openTable (FILE* f, const unsigned int nheadings, const unsigned int nwraps, const char *alignment, bool standalone);

/**
 * @brief Closes the latex table header
 * @param[out] f pointer to file to write
 * @param[in] flag if tex output should be standalone
 *
 */
void closeTable (FILE* f, bool standalone);


/**
 * @brief Creates the headings of the table
 * @param[out] f pointer to file to write
 * @param[in] nheadings Number of headings
 * @param[in] character string that is used to align, can be "l", "c" or "r".
 * @param[in] headings String array of headings 
 *
 */
void putHeader (FILE* f, const unsigned int nheadings, const unsigned int nwraps, const char *headings[]);

/**
 * @brief Makes sure only printable characters make it through. This includes escaping for special characters.
 * @param[in] entry character
 * @param[out] cleaned string
 * @returns pointer to str
 *
 */
char* cleanString (unsigned char entry, char *str);
