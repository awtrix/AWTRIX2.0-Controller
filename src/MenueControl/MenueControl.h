class MenueControl
{
  public:
    /*!
     * \brief This function give the text for the menue
     *
     * \param zeiger the actual menueNumber is needed as a adress (&...)
     * \return menue text
     */
    String getMenueString(int *zeiger,int *pressedTaster,int *minB,int *maxB);
};